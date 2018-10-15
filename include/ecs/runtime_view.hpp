#pragma once
#ifndef HEPHAESTUS_ENGINE_RUNTIME_ECS_VIEW_HPP
#define HEPHAESTUS_ENGINE_RUNTIME_ECS_VIEW_HPP
#include "sparse_set.hpp"

namespace ecs {

    template<typename>
    class Registry;

    template<typename entity_type>
    class runtime_view {
        friend class Registry<entity_type>;
        
        using view_type = sparse_set<entity_type>;
        using underlying_iterator_type = typename view_type::const_iterator_type;
        using pattern_type = std::vector<const view_type*>;
        using extent_type = typename view_type::size_type;
        using traits_type = entity_traits<entity_type>;

        class iterator {
            friend class runtime_view<entity_type>;

            iterator(underlying_iterator_type _begin, underlying_iterator_type _end, const view_type* const* _first, const view_type* const* _last, 
                extent_type _extent) : begin{ _begin }, end{ _end }, first{ _first }, last{ _last }, extent{ _extent } {
                if (begin != end && !valid()) {
                    ++(*this);
                }
            }

            bool valid() const noexcept {
                const auto entity = *begin;
                const auto size = static_cast<extent_type>(entity & traits_type::entity_mask);

                return size < extent && std::all_of(first, last, [entity](const auto* view) {
                    return view->unsafe_check(entity);
                });
            }
        
        public:
            using difference_type = typename underlying_iterator_type::difference_type;
            using value_type = typename underlying_iterator_type::value_type;
            using pointer = typename underlying_iterator_type::pointer;
            using reference = typename underlying_iterator_type::reference;
            using iterator_category = std::forward_iterator_tag;

            iterator() noexcept = default;
            iterator(const iterator&) noexcept = default;
            iterator& operator=(const iterator&) noexcept = default;

            iterator& operator++() noexcept {
                return (++begin != end && !valid()) ? ++(*this) : *this;
            }

            iterator operator++(int) noexcept {
                iterator orig = *this;
                ++(*this);
                return orig;
            }

            bool operator==(const iterator& other) const noexcept {
                return other.begin = begin;
            }

            bool operator!=(const iterator& other) const noexcept {
                return other.begin != begin;
            }

            pointer operator->() const noexcept {
                return begin.operator->();
            }

            reference operator*() const noexcept {
                return begin.operator*();
            }

        private:
            underlying_iterator_type begin;
            underlying_iterator_type end;
            const view_type* const* first;
            const view_type* const* last;
            extent_type extent;
        };

        runtime_view(pattern_type others) noexcept : pools{ others } {

            const auto iter = std::min_element(pools.begin(), pools.end(), [](const auto* lhs, const auto* rhs) {
                return (!lhs && rhs) || (lhs && rhs && lhs->size() < rhs->size());
            });

            std::rotate(pools.begin(), iter, pools.end());

        }

        extent_type min() const noexcept {
            extent_type ext{ 0u };
            if (valid()) {
                const auto iter = std::min_element(pools.cbegin(), pools.cend(), [](const auto* lhs, const auto* rhs) {
                    return lhs->extent() < rhs->extent();
                });

                ext = (*iter)->extent();
            }

            return ext;
        }

        bool valid() const noexcept {
            return !pools.empty() && pools.front();
        }

    public:

        using size_type = typename view_type::size_type;
        using iterator_type = iterator;
        using const_iterator_type = iterator;

        runtime_view(const runtime_view&) = default;
        runtime_view(runtime_view&&) = default;
        runtime_view& operator=(const runtime_view&) = default;
        runtime_view& operator=(runtime_view&&) = default;

        size_type size() const noexcept {
            return valid() ? pools.front()->size() : 0u;
        }

        bool empty() const noexcept {
            return !valid() || pool.front()->empty();
        }

        const_iterator_type cbegin() const noexcept {
            const_iterator_type iter{};
            if (valid()) {
                const auto& pool = *pools.front();
                const auto* const* data = pools.data();
                iter = { pool.cbegin(), pool.cend(), data + 1, data + pools.size(), min() };
            }
            return iter;
        }

        const_iterator_type begin() const noexcept {
            return cbegin();
        }

        iterator_type begin() noexcept {
            return cbegin();
        }

        const_iterator_type cend() const noexcept {
            const_iterator_type iter{};
            if (valid()) {
                const auto& pool = *pools.front();
                iter = { pool.cend(), pool.cend(), nullptr, nullptr, min() };
            }
            return iter;
        }

        const_iterator_type end() const noexcept {
            return cend();
        }

        iterator_type end() noexcept {
            return cend();
        }

        bool contains(const entity_type entity) const noexcept {
            return valid() && std::all_of(pools.cbegin(), pools.cend(), [entity](const auto* view){
                return view->has(entity) && view->data()[view->get(entity)] == entity;
            });
        }

        template<typename Function>
        void for_each(Function fn) {
            std::for_each(cbegin(), cend(), fn);
        }

    private:
        std::vector<pattern_type> pools;
    };

}

#endif //!HEPHAESTUS_ENGINE_RUNTIME_ECS_VIEW_HPP
