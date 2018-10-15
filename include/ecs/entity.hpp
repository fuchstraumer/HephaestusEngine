#pragma once
#ifndef HEPHAESTUS_ENGINE_CORE_ENTITY_TYPE_HPP
#define HEPHAESTUS_ENGINE_CORE_ENTITY_TYPE_HPP
#include <cstdint>
#include <limits>

namespace ecs {

    template<typename T>
    struct entity_traits;

    template<>
    struct entity_traits<uint32_t> {
        using entity_type = std::uint32_t;
        using version_type = std::uint16_t;
        using difference_type = std::int64_t;
        static constexpr const std::uint32_t entity_mask = 0xFFFFF;
        static constexpr const std::uint32_t version_mask = 0xFFF;
        static constexpr const auto entity_shift = 20;
    };

    using entity_t = uint32_t;
    using entity_traits_t = entity_traits<entity_t>;
    static constexpr const entity_t INVALID_ENTITY = entity_t{ entity_traits_t::entity_mask | (entity_traits_t::version_mask << entity_traits_t::entity_shift) };

}

#endif //!HEPHAESTUS_ENGINE_CORE_ENTITY_TYPE_HPP
