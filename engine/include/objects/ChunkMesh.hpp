#pragma once
#ifndef H_ENGINE_CHUNK_MESH_HPP
#define H_ENGINE_CHUNK_MESH_HPP
#include "stdafx.h"
#include "common/Constants.hpp"
#include "engine/objects/Block.hpp"

class ChunkMesh {
public:

private:
    void setBlockLightingData(const uint32_t& x, const uint32_t& y, const uint32_t& z, std::array<BlockType, 27>& neighbor_blocks, 
        std::array<float, 27>& neighbor_shades) const;
    void getFaceVertices(const blockFace & face, block_vertex_t & v0, block_vertex_t & v1, block_vertex_t & v2, block_vertex_t & v3, 
        const size_t& texture_idx) const;
    void createBlockFace(const blockFace& face, const size_t& uv_idx, const glm::vec3 & pos);
    void createCube(const size_t & x, const size_t & y, const size_t & z, const bool & front_face, const bool & right_face, const bool & top_face, 
        const bool & left_face,  const bool & bottom_face, const bool & back_face, const size_t & uv_idx);
        
};

#endif //!H_ENGINE_CHUNK_MESH_HPP