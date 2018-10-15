#pragma once
#ifndef HEPHAESTUS_ENGINE_CORE_ECS_SPARSE_SET_HPP
#define HEPHAESTUS_ENGINE_CORE_ECS_SPARSE_SET_HPP
#include <utility>
#include <iterator>
#include <numeric>
#include <vector>
#include "entity.hpp"
#include "sort_algorithms.hpp"

namespace ecs {

    template<typename...>
    class sparse_set;

    template<typename entity_type>
    class sparse_set<entity_type> {
    protected:

        using storage_type = std::vector<entity_type>;

        class Iterator final {
            friend class sparse_set<entity_type>;
            using direct_access_type = const storage_type;
            using index_type = std::size_t;
            Iterator(direct_access_type* access, index_type idx) noexcept : direct(access), index(idx) {}

        public:

            using difference_type = entity_traits_t::difference_type;
            using value_type = const entity_type;
            using pointer = value_type * ;
            using reference = value_type & ;
            using iterator_category = std::random_access_iterator_tag;

            Iterator() noexcept = default;
            Iterator(const Iterator&) noexcept = default;
            Iterator& operator=(const Iterator&) noexcept = default;

            Iterator& operator++() noexcept {
                --index;
                return *this;
            }

            Iterator& operator++(int) noexcept {
                Iterator orig = *this;
                ++(*this);
                return orig;
            }

            Iterator& operator--() noexcept {
                ++index;
                return *this;
            }

            Iterator& operator--(int) noexcept {
                Iterator orig = *this;
                --(*this);
                return orig;
            }

            Iterator& operator+=(const difference_type val) noexcept {
                index -= val;
                return *this;
            }

            Iterator operator+(const difference_type val) const noexcept {
                return Iterator{ direct, index - val };
            }

            Iterator& operator-=(const difference_type val) noexcept {
                index += val;
                return *this;
            }

            Iterator operator-(const difference_type val) const noexcept {
                return Iterator{ direct, index + val };
            }

            difference_type operator-(const Iterator& other) const noexcept {
                return other.index - index;
            }

            reference operator[](const difference_type val) const noexcept {
                return (*direct)[index - val - 1];
            }

            bool operator==(const Iterator& other) const noexcept {
                return index == other.index;
            }

            bool operator!=(const Iterator& other) const noexcept {
                return index != other.index;
            }

            bool operator<(const Iterator& other) const noexcept {
                return index > other.index;
            }

            bool operator>(const Iterator& other) const noexcept {
                return index < other.index;
            }

            bool operator<=(const Iterator& other) const noexcept {
                return !(*this > other);
            }

            bool operator>=(const Iterator& other) const noexcept {
                return !(*this < other);
            }

            pointer operator->() const noexcept {
                return &(*direct)[index - 1];
            }

            reference operator*() const noexcept {
                return (*direct)[index - 1];
            }

        private:
            direct_access_type* direct;
            index_type index;
        };

    public:

        using size_type = std::size_t;
        using iterator_type = Iterator;
        using const_iterator_type = Iterator;

        sparse_set() noexcept = default;
        virtual ~sparse_set() noexcept = default;
        sparse_set(const sparse_set&) = delete;
        sparse_set& operator=(const sparse_set&) = delete;
        sparse_set(sparse_set&&) = default;
        sparse_set& operator=(sparse_set&&) = default;

        void reserve(const size_type capacity) {
            packedData.reserve(capacity);
        }

        size_type capacity() const noexcept {
            return sparseData.capacity();
        }

        size_type extent() const noexcept {
            return packedData.size();
        }

        size_type size() const noexcept {
            return sparseData.size();
        }

        bool empty() const noexcept {
            return sparseData.empty();
        }

        const entity_type* data() const noexcept {
            return sparseData.data();
        }

        const_iterator_type cbegin() const noexcept {
            const size_type pos = sparseData.size();
            return const_iterator_type{ &sparseData, pos };
        }

        const_iterator_type begin() const noexcept {
            return cbegin();
        }

        const_iterator_type cend() const noexcept {
            return const_iterator_type{ &sparseData, entity_traits_t::difference_type{} };
        }

        const_iterator_type end() const noexcept {
            return cend();
        }

        iterator_type begin() noexcept {
            const size_type pos = sparseData.size();
            return iterator_type{ &sparseData, pos };
        }

        iterator_type end() noexcept {
            return iterator_type{ &sparseData, int64_t{} };
        }

        const entity_type& operator[](const size_type idx) const noexcept {
            return cbegin()[idx];
        }

        bool has(const entity_type ent) const noexcept {
            const size_type pos = static_cast<size_type>(ent & entity_traits_t::entity_mask);
            return (pos < packedData.size()) && (packedData[pos] != INVALID_ENTITY);
        }

        bool unsafe_check(const entity_type ent) const noexcept {
            const size_type pos = static_cast<size_type>(ent & entity_traits_t::entity_mask);
            return (packedData[pos] != INVALID_ENTITY);
        }

        size_type get(const entity_type ent) const noexcept {
            return packedData[ent & entity_traits_t::entity_mask];
        }

        void construct(const entity_type ent) {
            const size_type pos = static_cast<size_type>(ent & entity_traits_t::entity_mask);
            if (!(pos < packedData.size())) {
                packedData.resize(pos + 1, INVALID_ENTITY);
            }

            packedData[pos] = static_cast<entity_type>(sparseData.size());
            sparseData.push_back(ent);
        }

        virtual void destroy(const entity_type ent) {
            const entity_type back = sparseData.back();
            entity_type& candidate = packedData[ent & entity_traits_t::entity_mask];
            packedData[back & entity_traits_t::entity_mask] = candidate;
            sparseData[candidate] = back;
            candidate = INVALID_ENTITY;
            sparseData.pop_back();
        }

        void swap(const size_type lhs, const size_type rhs) noexcept {
            auto& src = sparseData[lhs];
            auto& dst = sparseData[rhs];
            std::swap(packedData[src & entity_traits_t::entity_mask], packedData[dst & entity_traits_t::entity_mask]);
            std::swap(src, dst);
        }

        void sort_with_respect_to(const sparse_set& other) noexcept {
            auto from = other.cbegin();
            auto to = other.cend();

            size_type pos = sparseData.size() - 1;
            while (pos && from != to) {
                if (has(*from)) {
                    if (*from != sparseData[pos]) {
                        swap(pos, get(*from));
                    }

                    --pos;
                }

                ++from;
            }
        }

        virtual void reset() {
            packedData.clear();
            packedData.shrink_to_fit();
            sparseData.clear();
            sparseData.shrink_to_fit();
        }

    protected:

        storage_type sparseData;
        storage_type packedData;

    };

    template<typename entity_type, typename component_type>
    class sparse_set<entity_type, component_type> : public sparse_set<entity_type> {
        //using component_allocator_type = memory::memory_pool<std::conditional_t<std::integral_constant<bool, sizeof(component_type) <= sizeof(void*)>::value, memory::small_node_pool, memory::node_pool>>;
        using component_storage_type = std::vector<component_type>;

        using underlying_type = sparse_set<entity_type>;
        using traits_type = entity_traits_t;

        template<bool CONST>
        class iterator final {
        private:
            friend class sparse_set<entity_type, component_type>;
            using instance_data_type = std::conditional_t<CONST, const component_storage_type, component_storage_type>;
            using index_type = typename traits_type::difference_type;
            iterator(instance_data_type* data, index_type idx) noexcept : instance(data), index(idx) {}
        public:

            using difference_type = index_type;
            using value_type = std::conditional_t<CONST, const component_type, component_type>;
            using pointer = value_type * ;
            using reference = value_type & ;
            using iterator_category = std::random_access_iterator_tag;

            iterator() noexcept = default;
            iterator(const iterator&) noexcept = default;
            iterator& operator=(const iterator&) noexcept = default;
            iterator(iterator&&) noexcept = default;
            iterator& operator=(iterator&&) noexcept = default;

            iterator& operator++() noexcept {
                --index;
                return *this;
            }

            iterator operator++(int) noexcept {
                iterator orig = *this;
                ++(*this);
                return orig;
            }

            iterator& operator--() noexcept {
                ++index;
                return *this;
            }

            iterator operator--(int) noexcept {
                iterator orig = *this;
                --(*this);
                return orig;
            }

            iterator& operator+=(const difference_type val) noexcept {
                index -= val;
                return *this;
            }

            iterator operator+(const difference_type val) const noexcept {
                return iterator{ instance, index - val };
            }

            iterator& operator-=(const difference_type val) noexcept {
                index += val;
                return *this;
            }

            iterator operator-(const difference_type val) const noexcept {
                return iterator{ instance, index + val };
            }

            difference_type operator-(const iterator& other) const noexcept {
                return other.index - index;
            }

            bool operator==(const iterator& other) const noexcept {
                return index == other.index;
            }

            bool operator!=(const iterator& other) const noexcept {
                return index != other.index;
            }

            bool operator<(const iterator& other) const noexcept {
                return index > other.index;
            }

            bool operator>(const iterator& other) const noexcept {
                return index < other.index;
            }

            bool operator<=(const iterator& other) const noexcept {
                return !(*this > other);
            }

            bool operator>=(const iterator& other) const noexcept {
                return !(*this < other);
            }

            pointer operator->() const noexcept {
                return &(*instance)[index - 1];
            }

            reference operator*() const noexcept {
                return (*instance)[index - 1];
            }

            reference operator[](const difference_type val) const noexcept {
                return (*instance)[index - val - 1];
            }

        private:
            instance_data_type* instance{ nullptr };
            index_type index{ std::numeric_limits<index_type>::max() };
        };

    public:

        using iterator_type = iterator<false>;
        using const_iterator_type = iterator<true>;
        using size_type = underlying_type::size_type;

        sparse_set() noexcept = default;
        sparse_set(const sparse_set&) = delete;
        sparse_set& operator=(const sparse_set&) = delete;

        void reserve(const size_type capacity) {
            underlying_type::reserve(capacity);
            components.reserve(capacity);
        }

        const component_type* raw() const noexcept {
            return components.data();
        }

        component_type* raw() noexcept {
            return components.data();
        }

        const_iterator_type cbegin() const noexcept {
            const traits_type::difference_type pos = static_cast<traits_type::difference_type>(components.size());
            return const_iterator_type{ &components, pos };
        }

        const_iterator_type begin() const noexcept {
            const traits_type::difference_type pos = static_cast<traits_type::difference_type>(components.size());
            return const_iterator_type{ &components, pos };
        }

        const_iterator_type cend() const noexcept {
            return const_iterator_type{ &components, traits_type::difference_type{} };
        }

        const_iterator_type end() const noexcept {
            return const_iterator_type{ &components, traits_type::difference_type{} };
        }

        iterator_type begin() noexcept {
            const traits_type::difference_type pos = static_cast<traits_type::difference_type>(components.size());
            return iterator_type{ &components, pos };
        }

        iterator_type end() noexcept {
            return iterator_type{ &components, traits_type::difference_type{} };
        }

        const component_type& operator[](const size_type idx) const noexcept {
            return cbegin()[idx];
        }

        component_type& operator[](const size_type idx) noexcept {
            return begin()[idx];
        }

        const component_type& get(const entity_type ent) const noexcept {
            return components[underlying_type::get(ent)];
        }

        component_type& get(const entity_type ent) noexcept {
            return const_cast<component_type&>(const_cast<const sparse_set*>(this)->get(ent));
        }

        template<typename...Args>
        component_type& construct(const entity_type entity, Args&&...args) {
            sparse_set<entity_type>::construct(entity);
            if constexpr (std::is_aggregate_v<component_type>) {
                components.emplace_back(component_type{ std::forward<Args>(args)... });
            }
            else {
                components.emplace_back(std::forward<Args>(args)...);
            }
            return components.back();
        }

        void destroy(const entity_type ent) override {
            auto tmp = std::move(components.back());
            components[underlying_type::get(ent)] = std::move(tmp);
            components.pop_back();
            underlying_type::destroy(ent);
        }

        void reset() override {
            underlying_type::reset();
            components.clear();
            components.shrink_to_fit();
        }

        template<typename Compare, typename Sort = std_sort, typename...Args>
        void sort(Compare cmp, Sort sort = Sort{}, Args&&...args) {
            std::vector<size_type> copies(components.size());
            std::iota(copies.begin(), copies.end(), 0);

            sort(copies.begin(), copies.end(), [this, cmp = std::move(cmp)](const auto lhs, const auto rhs){
                return cmp(const_cast<const component_type&>(components[rhs]), const_cast<const component_type&>(components[lhs]));
            }, std::forward<Args>(args)... );

            const size_type last = copies.size();
            for (size_type pos = 0; pos < last; ++pos) {
                auto curr = pos;
                auto next = copies[curr];

                while (curr != next) {
                    const auto lhs = copies[curr];
                    const auto rhs = copies[next];
                    std::swap(components[lhs], components[rhs]);
                    underlying_type::swap(lhs, rhs);
                    copies[curr] = curr;
                    curr = next;
                    next = copies[curr];
                }
            }
        }

        void sort_with_respect_to(const sparse_set<entity_type>& other) noexcept {
            auto from = other.cbegin();
            auto to = other.cend();

            size_type pos = underlying_type::size() - 1;
            const auto* local = underlying_type::data();

            while (pos && from != to) {
                const auto curr = *from;

                if (underlying_type::has(curr)) {
                    if (curr != *(local + pos)) {
                        auto candidate = underlying_type::get(curr);
                        std::swap(components[pos], components[candidate]);
                        underlying_type::swap(pos, candidate);
                    }
                    --pos;
                }

                ++from;
            }
        }

    private:
        component_storage_type components;
    };

}

#endif //!HEPHAESTUS_ENGINE_CORE_ECS_SPARSE_SET_HPP
