#pragma once
#ifndef H_ENGINE_CHUNK_MESH_HPP
#define H_ENGINE_CHUNK_MESH_HPP
#include "common/Constants.hpp"
#include "Block.hpp"
#include "ecs/entity.hpp"
#include "glm/vec3.hpp"
#include <vulkan/vulkan.h>
#include <vector>

struct VulkanResource;

struct ChunkMeshComponent {

    struct vertex_t {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec3 UV;
    };

    std::vector<uint32_t> Indices;
    std::vector<vertex_t> Vertices;
    VulkanResource* VBO{ nullptr };
    VulkanResource* EBO{ nullptr };

    constexpr static VkVertexInputBindingDescription binding{ 0, sizeof(vertex_t), VK_VERTEX_INPUT_RATE_VERTEX };
    constexpr static VkVertexInputAttributeDescription attributes[3]{ 
        VkVertexInputAttributeDescription{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0 },
        VkVertexInputAttributeDescription{ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, sizeof(glm::vec3) },
        VkVertexInputAttributeDescription{ 2, 0, VK_FORMAT_R32G32B32A32_SFLOAT, 2 * sizeof(glm::vec3) }
    };

private:
    friend class ChunkMeshingSystem;
    uint32_t addVertex(vertex_t&& v);
};

class ChunkMeshingSystem {
public:

    static void GenerateMesh(const ecs::entity_t ent, ChunkMeshComponent& mesh);

private:
    // void setBlockLightingData(const uint32_t& x, const uint32_t& y, const uint32_t& z, std::array<BlockType, 27>& neighbor_blocks, std::array<float, 27>& neighbor_shades) const;

    static void getFaceVertices(const BlockFace& face, ChunkMeshComponent::vertex_t& v0, ChunkMeshComponent::vertex_t& v1, ChunkMeshComponent::vertex_t& v2, ChunkMeshComponent::vertex_t& v3,
        const size_t& texture_idx);
    static void createBlockFace(const BlockFace& face, const size_t& uv_idx, const glm::vec3 & pos, ChunkMeshComponent& cmp);
    static void createCube(const size_t & x, const size_t & y, const size_t & z, const bool & front_face, const bool & right_face, const bool & top_face, 
        const bool & left_face,  const bool & bottom_face, const bool & back_face, const size_t & uv_idx, ChunkMeshComponent& cmp);
        
};

#endif //!H_ENGINE_CHUNK_MESH_HPP