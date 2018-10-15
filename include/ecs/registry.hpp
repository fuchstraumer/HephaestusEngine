#pragma once
#ifndef HEPHAESTUS_ENGINE_CORE_ECS_REGISTRY_HPP
#define HEPHAESTUS_ENGINE_CORE_ECS_REGISTRY_HPP
#include "entity.hpp"
#include "identifier.hpp"
#include "sparse_set.hpp"
#include "view.hpp"
#include "runtime_view.hpp"
#include "signal/multicast_delegate.hpp"
#include <tuple>
#include <iterator>
#include <memory>

namespace ecs  {

    template<typename entity_type>
    class Registry {
    private:

        Registry() noexcept = default;
        ~Registry() noexcept = default;
        Registry(const Registry&) = delete;
        Registry& operator=(const Registry&) = delete;

        using tag_id = static_identifier<struct InternalRegistryTagID>;
        using component_id = static_identifier<struct InternalRegistryComponentID>;
        using handler_id = static_identifier<struct InternalRegistryHandlerID>;
        using traits_type = entity_traits_t;
        using signal_type = multicast_delegate_t<void(Registry& reg, const entity_type ent)>;

        template<typename component_type>
        struct component_pool : sparse_set<entity_type, component_type> {

            component_pool(Registry* _reg) noexcept : reg(_reg) {}

            template<typename...Args>
            component_type& construct(const entity_type ent, Args&&...args) {
                auto& comp = sparse_set<entity_type, component_type>::construct(ent, std::forward<Args>(args)...);
                if (constructed) {
                    constructed(*reg, ent);
                }
                return comp;
            }

            void destroy(const entity_type ent) override {
                if (destroyed) {
                    destroyed(*reg, ent);
                }
                sparse_set<entity_type, component_type>::destroy(ent);
            }

            template<typename LambdaFunc>
            void add_construction_listener(LambdaFunc lf) {
                constructed += lf;
            }

            template<typename LambdaFunc>
            void add_destruction_listener(LambdaFunc lf) {
                destroyed += lf;
            }

            void add_construction_listener(const delegate_t<void(Registry& reg, const entity_type ent)>& d) {
                constructed += d;
            }

            void add_destruction_listener(const delegate_t<void(Registry& reg, const entity_type ent)>& d) {
                destroyed += d;
            }

            void remove_construction_listener(const delegate_t<void(Registry& reg, const entity_type ent)>& d) {
                constructed.remove(d);
            }

            void remove_destruction_listener(const delegate_t<void(Registry& reg, const entity_type ent)>& d) {
                destroyed.remove(d);
            }

        private:
            Registry* reg;
            signal_type constructed;
            signal_type destroyed;
        };

        template<typename component_type>
        bool check_component_storage() {
            const auto component_idx = component_id::id<component_type>();
            return (component_idx < pools.size()) && (pools[component_idx]);
        }

        template<typename component_type> 
        void assure_component_storage() {
            const auto component_idx = component_id::id<component_type>();
            if (!(component_idx < pools.size())) {
                pools.resize(component_idx + 1);
            }

            auto& cpool = pools[component_idx];

            if (!cpool) {
                cpool = std::make_unique<component_pool<component_type>>(this);
            }
        }

        template<typename component_type>
        const component_pool<component_type>& get_pool() const noexcept {
            const auto component_idx = component_id::id<component_type>();
            return static_cast<component_pool<component_type>&>(*pools[component_idx]);
        }

        template<typename component_type>
        component_pool<component_type>& get_pool() noexcept {
            return const_cast<component_pool<component_type>&>(const_cast<const Registry*>(this)->get_pool<component_type>());
        }

    public:

        //template<typename T>
        //using storage_allocator_t = memory::memory_pool<std::conditional_t<std::integral_constant<bool, sizeof(T) <= sizeof(void*)>::value, memory::small_node_pool, memory::node_pool>>;
        template<typename T>
        using storage_type_t = std::vector<T>;

        using version_type = typename traits_type::version_type;
        using size_type = std::size_t;
        using tag_id_type = tag_id::id_type;
        using component_id_type = component_id::id_type;

        inline static Registry& get_registry() noexcept {
            static Registry reg;
            return reg;
        }

        template<typename tag_type>
        static tag_id_type get_tag_id() noexcept {
            return tag_id::id<tag_type>();
        }

        template<typename component_type>
        static component_id_type get_component_id() noexcept {
            return component_id::id<component_type>();
        }

        template<typename component_type>
        size_type num_components() const noexcept {
            return check_component_storage<component_type>() : 
        }

        size_type num_entities() const noexcept {
            return entities.size();
        }

        size_type num_entities_alive() const noexcept {
            return entities.size() - available;
        }

        template<typename component_type>
        void reserve(const size_type capacity) {
            assure_component_storage<component_type>();
            get_pool<component_type>().reserve(capacity);
        }

        void reserve(const size_type capacity) {
            entities.reserve(capacity);
        }

        template<typename component_type>
        size_type capacity() const noexcept {
            return check_component_storage<component_type>() ? get_pool<component_type>().capacity() : size_type{ 0 };
        }

        size_type capacity() const noexcept {
            return entities.capacity();
        }

        template<typename component_type>
        bool empty() const noexcept {
            return !check_component_storage<component_type>() || get_pool<component_type>().empty();
        }

        bool empty() const noexcept {
            return entities.empty();
        }

        bool alive(const entity_type entity) const noexcept {
            const size_type pos = static_cast<size_type>(entity & traits_type::entity_mask);
            return (pos < entities.size() && entities[pos] == entity);
        }

        entity_type entity(const entity_type entity) const noexcept {
            return entity & entity_traits<entity_type>::entity_mask;
        }

        version_type version(const entity_type entity) const noexcept {
            return static_cast<version_type>(entity >> traits_type::entity_shift);
        }

        version_type current_version(const entity_type entity) const noexcept {
            const size_type pos = static_cast<size_t>(entity & traits_type::entity_mask);
            return static_cast<version_type>(entities[pos] >> traits_type::entity_shift);
        }

        entity_type create() {
            entity_type entity{};
            if (available) {
                const entity_type ent = nextEntity;
                const entity_type version = entities[ent] & (traits_type::version_mask << traits_type::entity_shift);
                nextEntity = entities[ent] & traits_type::version_mask;
                entity = ent | version;
                entities[ent] = entity;
                --available;
            }
            else {
                entity = static_cast<entity_type>(entities.size());
                entities.push_back(entity);
            }

            return entity;
        }

        template<typename tag_type>
        void destroy_tagged();

        void destroy(const entity_type entity) {
            for (size_type pos = pools.size(); pos; --pos) {
                auto& pool = pools[pos - 1];
                if (pool && pool->has(entity)) {
                    pool->destroy(entity);
                }
            }

            const entity_type ent = entity & traits_type::entity_mask;
            const entity_type version = ((entity >> traits_type::entity_shift) + 1) << traits_type::entity_shift;
            const entity_type node = (available ? nextEntity : ((ent + 1) & traits_type::entity_mask)) | version;
            entities[ent] = node;
            nextEntity = ent;
            ++available;
        }

        bool orphan(const entity_type ent) const {
            bool orphan = true;
            for (size_type i = 0; i < pools.size() && orphan; ++i) {
                const auto& pool = pools[i];
                orphan = !(pool && pool->has(ent));
            }
            return orphan;
        }

        template<typename component_type, typename...Args>
        component_type& assign(const entity_type entity, Args&&...args) {
            assure_component_storage<component_type>();
            auto& pool = get_pool<component_type>();
            pool.construct(entity, std::forward<Args>(args)...);
            return pool.get(entity);
        }

        template<typename component_type>
        void remove(const entity_type entity) {
            const auto component_idx = component_id::id<component_type>();
            get_pool<component_type>().destroy(entity);
        }

        template<typename...component_types>
        bool has(const entity_type entity) {
            return ((check_component_storage<component_types>() && get_pool<component_types>().has(entity)) && ...);
        }

        template<typename component_type>
        const component_type& get(const entity_type entity) const noexcept {
            return get_pool<component_type>().get(entity);
        }

        template<typename component_type>
        component_type& get(const entity_type entity) noexcept {
            return const_cast<component_type&>(const_cast<const Registry*>(this)->get<component_type>(entity));
        }

        template<typename...component_types>
        std::enable_if_t<(sizeof...(component_types) > 1), std::tuple<const component_types&...>> get(const entity_type entity) const noexcept {
            return std::tuple<const component_types&...>{ get<component_types>(entity)... };
        }

        template<typename...component_types>
        std::enable_if_t<(sizeof...(component_types) > 1), std::tuple<component_types&...>> get(const entity_type entity) noexcept {
            return std::tuple<component_types&...>{ get<component_types>(entity)... };
        }

        template<typename component_type, typename...Args>
        component_type& replace(const entity_type entity, Args&&...args) {
            return (get<component_type>(entity) = component_type{ std::forward<Args>(args)... });
        }

        template<typename component_type>
        void add_construction_listener(const delegate_t<void(Registry& reg, const entity_type ent)>& d) {
            get_pool<component_type>().add_construction_listener(d);
        }

        template<typename component_type>
        void add_destruction_listener(const delegate_t<void(Registry& reg, const entity_type ent)>& d) {
            get_pool<component_type>().add_destruction_listener(d);
        }

        template<typename component_type>
        void remove_construction_listener(const delegate_t<void(Registry& reg, const entity_type)>& d) {
            get_pool<component_type>().remove_construction_listener(d);
        }

        template<typename component_type>
        void remove_destruction_listener(const delegate_t<void(Registry& reg, const entity_type ent)>& d) {
            get_pool<component_type>().remove_destruction_listener(d);
        }

        template<typename function_type>
        void for_each(function_type fn) const {
            if (available) {
                for (size_type pos = entities.size(); pos; --pos) {
                    const entity_type curr = static_cast<entity_type>(pos - 1);
                    const entity_type entity = entities[(size_type)curr];
                    const entity_type ent = entity & traits_type::entity_mask;
                    if (curr == ent) {
                        fn(ent);
                    }
                }
            }
            else {
                for (size_type pos = entities.size(); pos; --pos) {
                    fn(entities[pos - 1]);
                }
            }
        }

        template<typename component_type, typename Compare, typename Sort = std_sort, typename...Args>
        void sort(Compare cmp, Sort sort = Sort{}, Args&&...args) {
            assure_component_storage<component_type>();
            get_pool<component_type>().sort(std::move(cmp), std::move(sort), std::forward<Args>(args)...);
        }

        template<typename from_component_type, typename to_component_type>
        void sort_with_respect_to() {
            assure_component_storage<from_component_type>();
            assure_component_storage<to_component_type>();
            get_pool<to_component_type>().sort_with_respect_to(get_pool<from_component_type>());
        }

        template<typename...component_types>
        component_view<entity_type, component_types...> view() {
            (assure_component_storage<component_types>(), ...);
            return component_view{ get_pool<component_types>()... };
        }

        template<typename component_type>
        raw_component_view<entity_type, component_type> raw_view() {
            assure_component_storage<component_type>();
            return raw_component_view{ get_pool<component_type>()... };
        }

        template<typename Iterator>
        runtime_view<entity_type> runtime_view(Iterator first, Iterator last) {
            static_assert(std::is_convertible_v<typename std::iterator_traits<Iter>::value_type, component_id_type>, "Invalid component iterator type for runtime view!");
            std::vector<const sparse_set<entity_type>*> set(last - first);

            std::transform(first, last, set.begin(), [this](const component_id_type c_type){
                return c_type < pools.size() ? pools[c_type].get() : nullptr;
            });

            return runtime_view{std::move(set)};
        }

        void reset() {
            for_each([this](const auto ent) {
                this->destroy(ent);
            });
        }

    private:

        using sparse_set_ptr_t = std::unique_ptr<sparse_set<entity_type>>;
        storage_type_t<sparse_set_ptr_t> pools;
        storage_type_t<sparse_set_ptr_t> handlers;
        storage_type_t<entity_type> entities;
        size_type available{ 0 };
        entity_type nextEntity{ INVALID_ENTITY };

    };

    using default_registry_t = Registry<entity_t>;

}

#endif //!HEPHAESTUS_ENGINE_CORE_ECS_REGISTRY_HPP
