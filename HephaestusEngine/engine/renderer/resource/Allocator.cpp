#include "stdafx.h"
#include "Allocator.h"

namespace vulpes {

	// padding to inject at end of allocations ot test allocation system
	static constexpr size_t DEBUG_PADDING = 0;

	Allocation::Allocation(Allocator * alloc) : allocator(alloc), availSize(0), freeCount(0), Memory(VK_NULL_HANDLE), Size(0) {}

	void Allocation::Init(VkDeviceMemory & new_memory, const VkDeviceSize & new_size) {
		assert(Memory == VK_NULL_HANDLE);
		Memory = new_memory;
		Size = new_size;
		freeCount = 1;
		availSize = new_size;

		Suballocations.clear();
		availSuballocations.clear();

		// note/create suballocation defining our singular free region.
		Suballocation suballoc{ 0, new_size, SuballocationType::Free };
		Suballocations.push_back(std::move(suballoc));

		// add location of that suballocation to mapping vector
		auto suballoc_iter = Suballocations.end();
		availSuballocations.push_back(suballoc_iter);

	}

	ValidationCode Allocation::Validate() const {

		if (Memory == VK_NULL_HANDLE) {
			return ValidationCode::NULL_MEMORY_HANDLE;
		}

		if (Size == 0 || Suballocations.empty()) {
			return ValidationCode::ZERO_MEMORY_SIZE;
		}

		// expected offset of newest suballocation compared to previous
		VkDeviceSize calculated_offset = 0;
		// expected number of free suballocations, from traversing that list
		uint32_t calculated_free_suballocs = 0;
		// available size, taken as sum of avail size of all free suballocations.
		VkDeviceSize calculated_free_size = 0;
		// number of free suballocations that need to be registered in the list for these objects
		uint32_t num_suballocs_to_register = 0;
		// set as we iterate, true when prev suballoc was free (used to sort/swap entries)
		bool prev_entry_free = false;

		for (auto iter = Suballocations.cbegin(); iter != Suballocations.cend(); ++iter) {
			const Suballocation& curr = *iter;

			// if offset of current suballoc is wrong, then this allocation is invalid
			if (curr.offset != calculated_offset) {
				return ValidationCode::INCORRECT_SUBALLOC_OFFSET;
			}

			// two adjacent free entries is invalid: should be merged.
			const bool curr_free = (curr.type == SuballocationType::Free);
			if (curr_free && prev_entry_free) {
				return ValidationCode::NEED_MERGE_SUBALLOCS;
			}

			// update prev free status
			prev_entry_free = curr_free;

			if (curr_free) {
				calculated_free_size += curr.size;
				++calculated_free_suballocs;
				if (curr.size >= MinSuballocationSizeToRegister) {
					++num_suballocs_to_register; // this suballocation should be registered in the free list.
				}
			}

			calculated_offset += curr.size;
		}

		// Number of free suballocations in objects list doesn't match what our calculated value says we should have
		if (availSuballocations.size() != num_suballocs_to_register) {
			return ValidationCode::FREE_SUBALLOC_COUNT_MISMATCH;
		}

		VkDeviceSize last_entry_size = 0;
		for (size_t i = 0; i < availSuballocations.size(); ++i) {
			auto curr_iter = availSuballocations[i];

			// non-free suballoc in free list
			if (curr_iter->type != SuballocationType::Free) {
				return ValidationCode::USED_SUBALLOC_IN_FREE_LIST;
			}

			// sorting of free list is incorrect
			if (curr_iter->size < last_entry_size) {
				return ValidationCode::FREE_SUBALLOC_SORT_INCORRECT;
			}

			last_entry_size = curr_iter->size;
		}

		if (calculated_offset != Size) {
			return ValidationCode::FINAL_SIZE_MISMATCH;
		}

		if (calculated_free_size != availSize) {
			return ValidationCode::FINAL_FREE_SIZE_MISMATCH;
		}

		if (calculated_free_suballocs != freeCount) {
			return ValidationCode::FREE_SUBALLOC_COUNT_MISMATCH;
		}

		// shouldn't return this
		return ValidationCode::VALIDATION_PASSED;
	}

	bool Allocation::RequestSuballocation(const VkDeviceSize & buffer_image_granularity, const VkDeviceSize & allocation_size, const VkDeviceSize & allocation_alignment, SuballocationType allocation_type, SuballocationRequest * dest_request) {
		if (availSize < allocation_size) {
			// not enough space in this allocation object
			return false;
		}

		const size_t numFreeSuballocs = availSuballocations.size();

		// use lower_bound to find location of avail suballocation
		auto avail_iter = std::lower_bound(availSuballocations.data(), availSuballocations.data() + numFreeSuballocs, allocation_size, suballocIterCompare());
		for (size_t idx = avail_iter - availSuballocations.data(); idx < numFreeSuballocs; ++idx) {
			VkDeviceSize offset = 0;
			const auto iter = availSuballocations[idx];
			// Check allocation for validity
			bool allocation_valid = VerifySuballocation(buffer_image_granularity, allocation_size, allocation_alignment, allocation_type, iter, &offset);
			if (allocation_valid) {
				dest_request->freeSuballocation = iter;
				dest_request->offset = offset;
				return true;
			}
		}

		return false;
	}

	bool Allocation::VerifySuballocation(const VkDeviceSize & buffer_image_granularity, const VkDeviceSize & allocation_size, const VkDeviceSize & allocation_alignment, SuballocationType allocation_type, const suballocationList::const_iterator & dest_suballocation_location, VkDeviceSize * dest_offset) const {
		assert(allocation_size > 0);
		assert(allocation_type != SuballocationType::Free);
		assert(dest_suballocation_location != Suballocations.cend());
		assert(dest_offset != nullptr);

		const Suballocation& suballoc = *dest_suballocation_location;
		assert(suballoc.type != SuballocationType::Free);

		if (suballoc.size < allocation_size) {
			return;
		}

		*dest_offset = suballoc.offset;

		// Apply alignment
		*dest_offset = AlignUp<VkDeviceSize>(*dest_offset, allocation_alignment);

		// check previous suballocations for conflicts with buffer-image granularity, change alignment as needed
		if (buffer_image_granularity > 1) {
			bool conflict_found = false;
			// iterate backwards, since we're checking previous suballoations for alignment conflicts
			auto prev_suballoc_iter = dest_suballocation_location;
			while (prev_suballoc_iter != Suballocations.cbegin()) {
				--prev_suballoc_iter;
				const Suballocation& prev_suballoc = *prev_suballoc_iter;
				bool on_same_page = CheckBlocksOnSamePage(prev_suballoc.offset, prev_suballoc.size, *dest_offset, buffer_image_granularity);
				if (on_same_page) {
					conflict_found = CheckBufferImageGranularityConflict(prev_suballoc.type, allocation_type);
					if (conflict_found) {
						break;
					}
				}
				else {
					break;
				}
			}
			if (conflict_found) {
				// align up by a page size to get off the current page and remove the conflict.
				*dest_offset = AlignUp<VkDeviceSize>(*dest_offset, buffer_image_granularity);
			}
		}

		// calculate padding at beginning from offset
		const VkDeviceSize padding_begin = *dest_offset - suballoc.offset;

		// calculate required padding at end, assuming current suballoc isn't at end of memory object
		auto next_iter = dest_suballocation_location;
		++next_iter;
		const VkDeviceSize padding_end = (next_iter != Suballocations.cend()) ? DEBUG_PADDING : 0;

		// Can't allocate if padding at begin and end is greater than requested size.
		if (padding_begin + padding_end > allocation_size) {
			return false;
		}

		// We checked previous allocations for conflicts: now, we'll check next suballocations
		if(buffer_image_granularity > 1) {
			auto next_iter = dest_suballocation_location;
			++next_iter;
			while (next_iter != Suballocations.cend()) {
				const auto& next_suballoc = *next_iter;
				bool on_same_page = CheckBlocksOnSamePage(*dest_offset, allocation_size, next_suballoc.offset, buffer_image_granularity);
				if (on_same_page) {
					if (CheckBufferImageGranularityConflict(allocation_type, next_suballoc.type)) {
						return false;
					}
				}
				else {
					break;
				}
				++next_iter;
			}
		}

		return true;
	}

	bool Allocation::Empty() const {
		return Suballocations.empty();
	}

	void Allocation::Allocate(const SuballocationRequest & request, const SuballocationType & allocation_type, const VkDeviceSize & allocation_size) {
		assert(request.freeSuballocation != Suballocations.cend());
		Suballocation& suballoc = *request.freeSuballocation;
		assert(suballoc.type == SuballocationType::Free); 

		const VkDeviceSize padding_begin = request.offset - suballoc.offset;
		const VkDeviceSize padding_end = suballoc.size - padding_begin - allocation_size;

		removeFreeSuballocation(request.freeSuballocation);

		suballoc.offset = request.offset;
		suballoc.size = allocation_size;
		suballoc.type = allocation_type;

		// if there's any remaining memory after this allocation, register it

		if (padding_end) {
			Suballocation padding_suballoc{ request.offset + allocation_size, padding_end, SuballocationType::Free };
			auto next_iter = request.freeSuballocation;
			++next_iter;
			const auto insert_iter = Suballocations.insert(next_iter, padding_suballoc);
			// insert_iter returns iterator giving location of inserted item
			insertFreeSuballocation(insert_iter);
		}

		// if there's any remaining memory before the allocation, register it.
		if (padding_begin) {
			Suballocation padding_suballoc{ request.offset - padding_begin, padding_begin, SuballocationType::Free };
			auto next_iter = request.freeSuballocation;
			++next_iter;
			const auto insert_iter = Suballocations.insert(next_iter, padding_suballoc);
			insertFreeSuballocation(insert_iter);
		}

	}

	void Allocation::mergeFreeWithNext(const suballocationList::iterator & item_to_merge) {
		auto next_iter = item_to_merge;
		++next_iter;
		assert(next_iter != Suballocations.cend());
		// add item to merge's size to the size of the object after it
		next_iter->size += item_to_merge->size;
		--freeCount;
		Suballocations.erase(item_to_merge);
	}

	void Allocation::freeSuballocation(const suballocationList::iterator & item_to_free) {
		Suballocation& suballoc = *item_to_free;
		suballoc.type = SuballocationType::Free;

		bool merge_next = false, merge_prev = false;

		auto next_iter = item_to_free;
		++next_iter;
		if ((next_iter != Suballocations.cend()) && (next_iter->type == SuballocationType::Free)) {
			merge_next = true;
		}

		auto prev_iter = item_to_free;
		
		if (prev_iter != Suballocations.cbegin()) {
			--prev_iter;
			if (prev_iter->type == SuballocationType::Free) {
				merge_prev = true;
			}
		}
	}

	void Allocation::insertFreeSuballocation(const suballocationList::iterator & item_to_insert) {
		if (item_to_insert->size >= MinSuballocationSizeToRegister) {
			if (availSuballocations.empty()) {
				availSuballocations.push_back(item_to_insert);
			}
			else {
				// find correct position ot insert "item_to_insert" and do so.
				auto insert_iter = std::lower_bound(availSuballocations.begin(), availSuballocations.end(), item_to_insert, suballocIterCompare());
				availSuballocations.insert(insert_iter, item_to_insert);
			}
		}
		++freeCount;
		availSize += item_to_insert->size;
	}

	void Allocation::removeFreeSuballocation(const suballocationList::iterator & item_to_remove) {
		if (item_to_remove->size >= MinSuballocationSizeToRegister) {
			auto remove_iter = std::remove(availSuballocations.begin(), availSuballocations.end(), item_to_remove);
			availSuballocations.erase(remove_iter, availSuballocations.end());
		}
		--freeCount;
		availSize -= item_to_remove->size;
	}


}