#pragma once
#ifndef HEPHAESTUS_ENGINE_CORE_ECS_IDENTIFIER_HPP
#define HEPHAESTUS_ENGINE_CORE_ECS_IDENTIFIER_HPP
#include <cstddef>
#include <atomic>

namespace ecs {

    template<typename...>
    class static_identifier {
        inline static std::atomic<std::size_t> identifier{ 0 };
        template<typename...>
        static std::size_t fetch_value() noexcept {
            static const std::size_t val = identifier.fetch_add(1);
            return val;
        }
    public:
        using id_type = std::size_t;
        template<typename...Type>
        inline static id_type id() noexcept {
            return fetch_value<std::decay_t<Type>...>();
        }
    };
    
    template<typename...>
    class runtime_identifier {
    private:
        inline static std::atomic<std::size_t> identifier{ 0 };
        template<typename...>
        static std::size_t fetch_value() noexcept {
            const std::size_t val = identifier.fetch_add(1);
            return val;
        }
    public:
        using id_type = std::size_t;
        template<typename...Type>
        inline static id_type id() noexcept {
            return fetch_value<std::decay_t<Type>...>();
        }
    };

}

#endif //!HEPHAESTUS_ENGINE_CORE_ECS_IDENTIFIER_HPP
