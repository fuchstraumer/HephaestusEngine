#pragma once
#ifndef CHUNK_H
#define CHUNK_H
#include "stdafx.h"
#include "Block.h"
#include "..\mesh\mesh.h"
#include "../generation/NoiseGen.h"
namespace objects {

	class Chunk {
		friend class ChunkManager;
	public:

		Chunk(glm::ivec2 grid_position);
		void BuildTerrain(const size_t& terrain_type);
		void BuildMesh();

		void CreateMeshBuffers(const vulpes::Device * _device);

		void LoadComplete(); // frees cpu data.

		// Position in homogenous integer grid defining chunk layout (used for logic, mostly)
		glm::ivec2 GridPosition;
		// Floating-point position used for rendering 
		glm::vec3 Position;

	private:

		noise::NoiseGenerator noiseGen;

		enum class blockFace : size_t {
			FRONT,
			RIGHT,
			TOP,
			LEFT,
			BOTTOM,
			BACK,
		};

		void setBlockLightingData(const uint32_t&, const uint32_t& y, const uint32_t& z, std::array<BlockType, 27>& neighbor_blocks, std::array<float, 27>& neighbor_shades) const;
		void getFaceVertices(const blockFace& face, block_vertex_t& v0, block_vertex_t& v1, block_vertex_t& v2, block_vertex_t& v3, const size_t& texture_idx);
		void createBlockFace(const blockFace& face, const size_t& uv_idx, const glm::vec3& pos);

		void createCube(const size_t& x, const size_t& y, const size_t& z, const bool& front_face, const bool& right_face,
			const bool& top_face, const bool& leftFace, const bool& bottom_face, const bool& backFace, const size_t& uv_idx);

		bool generated = false;
		std::array<Block, BLOCKS_PER_CHUNK> terrainBlocks;
		std::unique_ptr<mesh::Mesh<mesh::BlockVertices, block_vertex_t>> mesh;

	};

}
#endif // !LINEAR_CHUNK_H
