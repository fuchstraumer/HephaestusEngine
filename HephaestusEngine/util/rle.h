#include <vector>
#include <iterator>
#ifndef RLE_H
#define RLE_H
#include "../stdafx.h"

using dataType = blockType;
using dataContainer = std::vector<dataType>;
using dataIterator = dataContainer::const_iterator;


// Get the counter bits
static const dataType counterBits = std::numeric_limits<dataType>::max() / 2;
// Get the repetition bits, or the binary inversion of the counterBits
static const dataType repetitionBit = ~counterBits;

inline auto countRepetitions(dataIterator iter, dataIterator end) {
	const auto first = iter;
	// While our iter isn't at the end and our values are equal, iterate
	// down iter until we find a new value. Return the distance we iterated 
	// down iter compared to our starting value "*first"
	do ++iter;
		while (iter != end && *iter == *first);
		
	return std::distance(first, iter);
}

inline auto countUniques(dataIterator iter, dataIterator end) {
	const auto first = iter;
	// Make sure to check the range
	// while not at the end and while the value of iter + 1
	// isn't equal to the current value of iter
	while ((iter + 1) != end && *(iter + 1) != *iter)
		++iter;
	return std::distance(first, iter);
}

// This divides our N-items into chunks of the maximum size permitted by counter bits
template<typename T>
inline auto split(std::ptrdiff_t num, T&& t) {
	// ptrdiff is the standard type of the difference between two iterators
	do {
		// Our count is the smallest of these two. This is only okay because we always know 
		// our count can never be negative. At worst, it'll be 0 or 1.
		auto count = std::min(static_cast<unsigned int>(num), static_cast<unsigned int>(counterBits));
		t(count);
		num -= count;
	} while (num > 0);
}

inline dataContainer encode(const dataContainer &data) {
	dataContainer out;
	out.reserve(data.size());
	for (auto iter = data.cbegin(); iter != data.cend();) {
		// First attempt at checking for repetitions
		auto num = countRepetitions(iter, data.cend());
		// If there's more than one repetition, we have a run
		if (num > 1) {
			split(num, [&](blockType count) {
				// Repetition bit plus the count
				out.emplace_back(repetitionBit | count);
				// Insert the value that's being repeated
				out.emplace_back(*iter);
			});
			// Iterate iter by the number of repetitions of *iter
			iter += num;
		}
		else {
			// Count amount of non-repeated characters in a row/run (unique chars)
			auto num = countUniques(iter, data.cend());
			split(num, [&](blockType count) {
				// No repeition bit, just counter
				out.emplace_back(count);
				// Instead of having to loop over the iter, we can
				// just copy values from one vector to another.
				std::copy(iter, iter + count, std::back_inserter(out));
				// Iterate down the data by how many unique chars we found
				iter += count;
			});
		}
	}
	out.shrink_to_fit();
	return out;
}

inline dataContainer decode(const dataContainer &data) {
	dataContainer out; // No good way to reserve - can't guess 
	// size of decompressed data very easily.
	for (auto iter = data.cbegin(); iter != data.cend();) {
		bool repeat = (*iter & repetitionBit) > 0;
		auto count = *iter & counterBits;
		if (repeat) {
			for (auto i = 0; i < count;) {
				out.emplace_back(*iter);
			}
			++iter;
		}
		else {
			++iter;
			for (; count > 1; count--) {
				out.emplace_back(*++iter);
			}
		}
	}
	return out;
}


#endif // !RLE_H
