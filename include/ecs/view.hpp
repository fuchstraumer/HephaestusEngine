#pragma once
#ifndef HEPHAESTUS_ENGINE_CORE_ECS_VIEWS_HPP
#define HEPHAESTUS_ENGINE_CORE_ECS_VIEWS_HPP
#include <type_traits>
#include <tuple>
#include <array>
#include "entity.hpp"
#include "sparse_set.hpp"

namespace ecs {

    template<typename entity_type>
    class Registry;

    template<typename entity_type, typename...component_types>
    class persistent_view {
    private:
        static_assert(sizeof...(component_types) > 1, "PersistentViews must be created with more than one component type.");
        friend class Registry<entity_type>;

        template<typename component_type>
        using storage_type = sparse_set<entity_type, component_type>;

        using view_type = sparse_set<entity_type>;
        using storage_tuple_type = std::tuple<storage_type<component_types>&...>;

        persistent_view(view_type& _view, storage_type<component_types> &... _storage) noexcept : view{ _view }, storage{ _storage... } {}
    public:

        using size_type = typename view_type::size_type;
        using iterator_type = typename view_type::iterator_type;
        using const_iterator_type = typename view_type::const_iterator_type;

        size_type size() const noexcept {
            return view.size();
        }

        bool empty() const noexcept {
            return view.empty();
        }

        const entity_type* data() const noexcept {
            return view.data();
        }

        const_iterator_type cbegin() const noexcept {
            return view.cbegin();
        }

        const_iterator_type begin() const noexcept {
            return view.begin();
        }

        const_iterator_type cend() const noexcept {
            return view.cend();
        }

        const_iterator_type end() const noexcept {
            return view.end();
        }

        iterator_type begin() noexcept {
            return view.begin();
        }

        iterator_type end() noexcept {
            return view.end();
        }

        const entity_type& operator[](const size_type idx) const noexcept {
            return view[idx];
        }

        bool contains(const entity_type entity) const noexcept {
            return view.has(entity) && (view.data()[view.get(entity)] == entity);
        }

        template<typename...component_types>
        std::conditional_t<sizeof...(component_types) == 1, 
            std::tuple_element_t<0, std::tuple<const component_types&...>>, std::tuple<const component_types&...>> get(const entity_type entity) const noexcept {
            if constexpr (sizeof...(component_types) == 1) {
                return (std::get<storage_type<component_types>&>(storage).get(entity), ...);
            }
            else {
                return std::tuple<const component_types&...>{ get<component_types>(entity)... };
            }
        }

        template<typename...component_types>
        std::conditional_t<sizeof...(component_types) == 1,
            std::tuple_element_t<0, std::tuple<component_types&...>>, std::tuple<component_types&...>> get(const entity_type entity) noexcept {
            if constexpr (sizeof...(component_types) == 1) {
                return (const_cast<component_types&>(std::as_const(*this).template get<component_types>(entity)), ...);
            }
            else {
                return std::tuple<component_types&...>{ get<component_types>(entity)... };
            }
        }

        template<typename function_type>
        void for_each(function_type fn) const {
            std::for_each(view.cbegin(), view.cend(), [&fn, this](const auto ent) {
                fn(ent, std::get<storage_type<component_types>&>(pools).get(ent)...);
            });
        }

        template<typename function_type>
        void for_each(function_type fn) {
            std::for_each(view.begin(), view.end(), [&fn, this](const auto ent) {
                fn(ent, std::get<storage_type<component_types>&>(pools).get(ent)...);
            })
        }

        template<typename component_type>
        void sort() {
            view.sort_with_respect_to(std::get<storage_type<component_type>&>(storage));
        }

    private:
        view_type& view;
        const storage_tuple_type storage;
    };

    template<typename entity_type, typename...component_types>
    class component_view final {
    protected:

        static_assert((sizeof...(component_types) > 1), "This component view type requires more than once component type!");
        friend class Registry<entity_type>;

        template<typename component_type>
        using storage_type = sparse_set<entity_type, component_type>;

        template<typename component_type>
        using component_iterator_type = typename storage_type<component_type>::const_iterator_type;

        using view_type = sparse_set<entity_type>;
        using underlying_iterator_type = typename view_type::const_iterator_type;
        using unchecked_type = std::array<const view_type*, (sizeof...(component_types) - 1)>;
        using tuple_storage_type = std::tuple<storage_type<component_types> &...>;
        using traits_type = entity_traits<entity_type>;
        using size_type = typename view_type::size_type;

        class iterator {
        private:
            friend class component_view<entity_type, component_types...>;
            using extent_type = typename view_type::size_type;

            iterator(unchecked_type unchecked, underlying_iterator_type begin, underlying_iterator_type end) noexcept : unchecked{ unchecked },
                begin{ begin }, end{ end }, extent{ min(std::make_index_sequence<unchecked.size()>{}) } {
                if (begin != end && !valid()) {
                    ++(*this);
                }
            }

            template<std::size_t...Indices>
            extent_type min(std::index_sequence<Indices...>) const noexcept {
                return std::min({ std::get<Indices>(unchecked)->extent()... });
            }

            bool valid() const noexcept {
                const auto entity = *begin;
                const auto size = static_cast<size_type>(entity & traits_type::entity_mask);
                return size < extent && std::all_of(unchecked.cbegin(), unchecked.cend(), [entity](const view_type* view) {
                    return view->has(entity);
                });
            }

        public:

            using difference_type = typename traits_type::difference_type;
            using value_type = typename underlying_iterator_type::value_type;
            using pointer = typename underlying_iterator_type::pointer;
            using reference = typename underlying_iterator_type::reference;
            using iterator_category = std::forward_iterator_tag;

            iterator() noexcept = default;
            iterator(const iterator&) noexcept = default;
            iterator(iterator&&) noexcept = default;
            iterator& operator=(const iterator&) = default;
            iterator& operator=(iterator&&) noexcept = default;

            iterator& operator++() noexcept {
                return (++begin != end && !valid() ? ++(*this) : *this);
            }

            iterator operator++(int) noexcept {
                iterator original = *this;
                ++(*this);
                return original;
            }

            bool operator==(const iterator& other) const noexcept {
                return other.begin == begin;
            }

            bool operator!=(const iterator& other) const noexcept {
                return other.begin != begin;
            }

            pointer operator->() const noexcept {
                return begin.operator->();
            }

            reference operator*() const noexcept {
                return *begin;
            }

        private:
            unchecked_type unchecked;
            underlying_iterator_type begin;
            underlying_iterator_type end;
            extent_type extent;
        };

        component_view(storage_type<component_types> &... pools) noexcept : pools{ pools... } {}

        template<typename component_type>
        const storage_type<component_type>& pool() const noexcept {
            return std::get<storage_type<component_type>&>(pools);
        }

        // candiate view type for iteration is whichever of our component types has the smallest attached pool
        const view_type* candidate() const noexcept {
            return std::min({ static_cast<const view_type*>(&pool<component_types>())... }, [](const auto* lhs, const auto* rhs) {
                return lhs->size() < rhs->size();
            });
        }

        unchecked_type unchecked(const view_type* view) const noexcept {
            unchecked_type other{};
            std::size_t pos{};
            ((&pool<component_types>() == view ? nullptr : (other[pos++] = &pool<component_types>())), ...);
            return other;
        }

        template<typename component_type, typename other_component_type>
        const other_component_type& get(const component_iterator_type<component_type>& iter, const entity_type entity) const noexcept {
            if constexpr (std::is_same_v<component_type, other_component_type>) {
                return *iter;
            }
            else {
                return pool<other_component_type>().get(entity);
            }
        }

        template<typename component_type, typename function_type, std::size_t...Indices>
        void for_each(const storage_type<component_type>& pool, function_type fn, std::index_sequence<Indices...>) const {
            const auto other = unchecked(&pool);
            std::array<underlying_iterator_type, sizeof...(Indices)> data{ { std::get<Indices>(other)->cbegin()... } };
            const auto extent = std::min({ pool<component_type>().extent()... });
            auto raw = std::make_tuple(pool<component_type>().cbegin()...);
            const auto end = pool.view_type::cend();
            auto begin = pool.view_type::cbegin();

            while (((begin != end) && ... && (*begin == *(std::get<Indices>(data)++)))) {
                fn(*(begin++), *(std::get<component_iterator_type<component_type>>(raw)++)...);
            }

            while (begin != end) {
                const auto entity = *(begin++);
                const auto iter = std::get<component_iterator_type<component_type>>(raw)++;
                const size_type sz = static_cast<size_type>(entity & traits_type::entity_mask);

                if (((sz < extent) && ... && std::get<Indices>(other)->has(entity))) {
                    fn(entity, get<component_type, component_types>(iter, entity)...);
                }
            }
        }

    public:

        using size_type = typename view_type::size_type;
        using iterator_type = iterator;
        using const_iterator_type = iterator;

        size_type size() const noexcept {
            return std::min({ pool<component_types>().size()... });
        }

        bool empty() const noexcept {
            return (pool<component_types>().empty() || ...);
        }

        const_iterator_type cbegin() const noexcept {
            const auto* view = candidate();
            return const_iterator_type{ unchecked(view), view->cbegin(), view->cend() };
        }

        const_iterator_type begin() const noexcept {
            const auto* view = candidate();
            return const_iterator_type{ unchecked(view), view->cbegin(), view->cend() };
        }

        const_iterator_type cend() const noexcept {
            const auto* view = candidate();
            return const_iterator_type{ unchecked(view), view->cend(), view->cend() };
        }

        const_iterator_type end() const noexcept {
            const auto* view = candidate();
            return const_iterator_type{ unchecked(view), view->cend(), view->cend() };
        }

        bool contains(const entity_type entity) const noexcept {
            const size_type pos = static_cast<size_type>(entity & traits_type::entity_mask);
            const size_type extent = std::min({ pool<component_types>().extent()... });
            return ((pos < extent) && ... && (pool<component_types>().has(entity) && (pool<component_types>().data()[pool<component_types>().view_type::get(entity)] == entity)));
        }

        template<typename...selected_component_types>
        std::conditional_t<sizeof...(selected_component_types) == 1, 
            std::tuple_element_t<0, std::tuple<const selected_component_types&...>>, std::tuple<const selected_component_types&...>> get(const entity_type entity) const noexcept {
            if constexpr (sizeof...(selected_component_types) == 1) {
                return (pool<selected_component_types>().get(entity), ...);
            }
            else {
                return std::tuple<const selected_component_types&...>{ get<selected_component_types>(entity)... };
            }
        }

        template<typename...selected_component_types>
        std::conditional_t<sizeof...(selected_component_types) == 1,
            std::tuple_element_t<0, std::tuple<selected_component_types&...>>, std::tuple<selected_component_types&...>> get(const entity_type entity) noexcept {
            if constexpr (sizeof...(selected_component_types) == 1) {
                return (const_cast<selected_component_types&>(std::as_const(*this).template get<selected_component_types>(entity)), ...);
            }
            else {
                return std::tuple<selected_component_types&...>{ get<selected_component_types>(entity)... };
            }
        }

        // signature of calling function should be:
        // void(const entity_type ent, const component_types&...)
        template<typename function_type>
        void for_each(function_type fn) const {
            const auto* view = candidate();
            // only apply function if we are able to, given extent/size of current pool (as we iterate through the pack)
            ((&pool<component_types>() == view ? for_each(pool<component_types>(), std::move(fn), std::make_index_sequence<sizeof...(component_types) - 1>{}) : void()), ...);
        }

        template<typename function_type>
        void for_each(function_type fn) {
            std::as_const(*this).for_each([&fn](const entity_type entity, const component_types &... components) {
                fn(entity, const_cast<component_types&>(components)...);
            });
        }

    private:
        const tuple_storage_type pools;
    };

    template<typename entity_type, typename component_type>
    class component_view<entity_type, component_type> final {
    private:

        friend class Registry<entity_type>;
        using view_type = sparse_set<entity_type>;
        using storage_type = sparse_set<entity_type, component_type>;

        component_view(storage_type& _pool) noexcept : pool{ _pool } {}

    public:

        using value_type = component_type;
        using size_type = typename storage_type::size_type;
        using iterator_type = typename view_type::iterator_type;
        using const_iterator_type = typename view_type::const_iterator_type;

        size_type size() const noexcept {
            return pool.size();
        }

        bool empty() const noexcept {
            return pool.empty();
        }

        const value_type* raw() const noexcept {
            return pool.raw();
        }

        value_type* raw() noexcept {
            return const_cast<component_type*>(std::as_const(*this).raw());
        }

        const entity_type* data() const noexcept {
            return pool.data();
        }

        const_iterator_type cbegin() const noexcept {
            return pool.view_type::cbegin();
        }

        const_iterator_type begin() const noexcept {
            return pool.view_type::begin();
        }

        const_iterator_type cend() const noexcept {
            return pool.view_type::cend();
        }

        const_iterator_type end() const noexcept {
            return pool.view_type::end();
        }

        iterator_type begin() noexcept {
            return pool.view_type::begin();
        }

        iterator_type end() noexcept {
            return pool.view_type::end();
        }

        const entity_type& operator[](const size_type pos) const noexcept {
            return pool.view_type::operator[](pos);
        }

        bool contains(const entity_type entity) const noexcept {
            return pool.has(entity) && (pool.data()[pool.view_type::get(entity)] == entity);
        }

        const value_type& get(const entity_type entity) const noexcept {
            return pool.get(entity);
        }

        value_type& get(const entity_type entity) noexcept {
            return const_cast<value_type&>(std::as_const(*this).get(entity));
        }

        // function signature should be: void(const entity_type, const component_type&)
        template<typename function_type>
        void for_each(function_type fn)  const {
            std::for_each(pool.view_type::begin(), pool.view_type::cend(), [&fn, raw = pool.cbegin()](const auto entity) mutable {
                fn(entity, *(raw++));
            });
        }

        // function signature should be: void(const entity_type, component_type&)
        template<typename function_type>
        void for_each(function_type fn) {
            std::as_const(*this).for_each([&fn](const entity_type entity, const component_type& component) {
                fn(entity, const_cast<component_type&>(component));
            });
        }

    private:
        const storage_type& pool;
    };

    template<typename entity_type, typename component_type>
    class raw_component_view final {
    private:

        friend class Registry<entity_type>;

        using storage_type = sparse_set<entity_type, component_type>;

        raw_component_view(storage_type& _pool) noexcept : pool{ _pool } {}

    public:

        using value_type = component_type;
        using size_type = typename storage_type::size_type;
        using iterator_type = typename storage_type::iterator_type;
        using const_iterator_type = typename storage_type::const_iterator_type;

        size_type size() const noexcept {
            return pool.size();
        }

        bool empty() const noexcept {
            return pool.empty();
        }

        const value_type* raw() const noexcept {
            return pool.raw();
        }

        value_type* raw() noexcept {
            return const_cast<value_type*>(std::as_const(*this).raw());
        }

        const entity_type* data() const noexcept {
            return pool.data();
        }

        const_iterator_type cbegin() const noexcept {
            return pool.cbegin();
        }

        const_iterator_type begin() const noexcept {
            return pool.begin();
        }

        const_iterator_type cend() const noexcept {
            return pool.cend();
        }

        const_iterator_type end() const noexcept {
            return pool.end();
        }

        iterator_type begin() noexcept {
            return pool.begin();
        }

        iterator_type end() noexcept {
            return pool.end();
        }

        const value_type& operator[](const size_type idx) const noexcept {
            return pool[idx];
        }

        value_type& operator[](const size_type idx) noexcept {
            return const_cast<value_type&>(std::as_const(*this).operator[](idx));
        }

        template<typename function_type>
        void for_each(function_type fn) const {
            std::for_each(pool.cbegin(), pool.cend(), fn);
        }

        template<typename function_type>
        void for_each(function_type fn) {
            std::for_each(pool.begin(), pool.end(), fn);
        }

    private:
        storage_type& pool;
    };

}

#endif // !HEPHAESTUS_ENGINE_CORE_ECS_VIEWS_HPP
