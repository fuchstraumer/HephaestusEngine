
set(engine_common_headers 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/common/BlockTypes.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/common/Constants.hpp"
)

set(engine_ecs_sources
    "${CMAKE_CURRENT_SOURCE_DIR}/include/ecs/entity.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/ecs/identifier.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/ecs/registry.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/ecs/runtime_view.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/ecs/sort_algorithms.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/ecs/sparse_set.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/ecs/view.hpp"
)

set(engine_generation_sources
    "${CMAKE_CURRENT_SOURCE_DIR}/include/generation/NoiseGen.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/generation/TerrainGenerator.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/generation/NoiseGen.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/generation/TerrainGenerator.cpp"
)

set(engine_object_sources
    
)
