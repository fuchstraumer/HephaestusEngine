
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
    "${CMAKE_CURRENT_SOURCE_DIR}/include/objects/Block.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/objects/BlockTypeDescription.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/objects/Chunk.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/objects/ChunkManager.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/objects/ChunkMesh.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/objects/Chunk.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/objects/ChunkManager.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/objects/ChunkMesh.cpp"
)

source_group("common" FILES ${engine_common_headers})
source_group("ecs" FILES ${engine_ecs_sources})
source_group("generation" FILES ${engine_generation_sources})
source_group("objects" FILES ${engine_object_sources})

ADD_LIBRARY(HephaestusEngine STATIC ${engine_common_headers} ${engine_ecs_sources} ${engine_generation_sources} ${engine_object_sources})
SET_COMPILER_OPTIONS(HephaestusEngine)

TARGET_LINK_LIBRARIES(HephaestusEngine INTERFACE resource_context rendering_context)

TARGET_INCLUDE_DIRECTORIES(HephaestusEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" 
    "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/diamond_dogs/modules/rendering_context/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/diamond_dogs/modules/resource_context/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/diamond_dogs/third_party/glm/"
)
