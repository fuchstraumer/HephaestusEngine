#include "objects/ChunkMesh.hpp"
#include "objects/Chunk.hpp"
#include "objects/Block.hpp"
#include "util/Morton.hpp"
#include "ecs/registry.hpp"
#include <array>

// Face normals. Don't change and can be reused. Yay for cubes!
static const std::array<glm::ivec3, 6> normals {
	glm::ivec3( 0, 0, 1),   // (front)
	glm::ivec3( 1, 0, 0),   // (right)
	glm::ivec3( 0, 1, 0),   // (top)
	glm::ivec3(-1, 0, 0),   // (left)
	glm::ivec3( 0,-1, 0),   // (bottom)
	glm::ivec3( 0, 0,-1),   // (back)
};

static const std::array<glm::vec3, 8> base_vertices{
	glm::vec3(-0.50f,-0.50f, 0.50f), // Point 0, left lower front
	glm::vec3( 0.50f,-0.50f, 0.50f), // Point 1, right lower front
	glm::vec3( 0.50f, 0.50f, 0.50f), // Point 2, right upper front 
	glm::vec3(-0.50f, 0.50f, 0.50f), // Point 3, left upper front
	glm::vec3( 0.50f,-0.50f,-0.50f), // Point 4, right lower rear
	glm::vec3(-0.50f,-0.50f,-0.50f), // Point 5, left lower rear
	glm::vec3(-0.50f, 0.50f,-0.50f), // Point 6, left upper rear
	glm::vec3( 0.50f, 0.50f,-0.50f), // Point 7, right upper rear
};

constexpr static std::array<std::initializer_list<uint32_t>, 6> face_indices{
	std::initializer_list<uint32_t>{ 5, 4, 1, 0 }, // Bottom
	std::initializer_list<uint32_t>{ 3, 2, 7, 6 }, // Top
	std::initializer_list<uint32_t>{ 4, 5, 6, 7 }, // Back
	std::initializer_list<uint32_t>{ 0, 1, 2, 3 }, // Front 
	std::initializer_list<uint32_t>{ 5, 0, 3, 6 }, // Left
	std::initializer_list<uint32_t>{ 1, 4, 7, 2 } // Right
};

constexpr static size_t textures[24][6] = {
	// Each number corresponds to certain face, and thus certain index into texture array. Given as {front, right, top, left, bottom, back}
	// Current array order: Bedrock, Grass Top, Grass Sides, Dirt, Stone, Gravel, Sand, Cobble, Coal, Iron, Gold, Diamond, Emerald, Log, Log Top,
	// Leaves, Planks, Glass, Stonebricks, Bricks, Tall grass, Fern, Flower, Grass Lower, Grass Upper,
	{  0, 0, 0, 0, 0, 0 }, // Bedrock block
	{  1, 1, 1, 1, 1, 1 }, // Stone
	{  2, 2, 1, 2, 3, 2 }, // Grass block
	{  5, 5, 5, 5, 5, 5 }, // Sand block
	{  3, 3, 3, 3, 3, 3 }, // Dirt block
	{  5, 5, 5, 5, 5, 5 }, // Gravel
	{  6, 6, 6, 6, 6, 6 }, // Sand
	{  7, 7, 7, 7, 7, 7 }, // Cobblestone
	{  8, 8, 8, 8, 8, 8 }, // Coal
	{  9, 9, 9, 9, 9, 9 }, // Iron
	{ 10,10,10,10,10,10 }, // Gold
	{ 11,11,11,11,11,11 }, // Diamond
	{ 12,12,12,12,12,12 }, // Emerald
	{ 20,20,20,20,20,20 }, // tall grass
};

/*
    The following look-up-tables are used to check a distance from a point
    without incurring a heavy computational expense
*/

constexpr static std::array<int32_t, 256> deltaX = {
    1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1,
    3511, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1,
    7, 1, 28087, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1,
    55, 1, 7, 1, 3511, 1, 7, 1, 55, 1, 7, 1, 439, 1,
    7, 1, 55, 1, 7, 1, 224695, 1, 7, 1, 55, 1, 7, 1,
    439, 1, 7, 1, 55, 1, 7, 1, 3511, 1, 7, 1, 55, 1,
    7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 28087, 1, 7, 1,
    55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 3511, 1,
    7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 1797559,
    1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1,
    3511, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1,
    7, 1, 28087, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1,
    55, 1, 7, 1, 3511, 1, 7, 1, 55, 1, 7, 1, 439, 1,
    7, 1, 55, 1, 7, 1, 224695, 1, 7, 1, 55, 1, 7, 1,
    439, 1, 7, 1, 55, 1, 7, 1, 3511, 1, 7, 1, 55, 1,
    7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 28087, 1, 7, 1,
    55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 3511, 1,
    7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1
};

static const std::array<int32_t, 256> deltaY = {
    2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2,
    14, 2, 7022, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14,
    2, 110, 2, 14, 2, 56174, 2, 14, 2, 110, 2, 14, 2,
    878, 2, 14, 2, 110, 2, 14, 2, 7022, 2, 14, 2, 110,
    2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 449390, 2,
    14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2,
    7022, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110,
    2, 14, 2, 56174, 2, 14, 2, 110, 2, 14, 2, 878, 2,
    14, 2, 110, 2, 14, 2, 7022, 2, 14, 2, 110, 2, 14,
    2, 878, 2, 14, 2, 110, 2, 14, 2, 3595118, 2, 14,
    2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2,
    7022, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110,
    2, 14, 2, 56174, 2, 14, 2, 110, 2, 14, 2, 878, 2,
    14, 2, 110, 2, 14, 2, 7022, 2, 14, 2, 110, 2, 14,
    2, 878, 2, 14, 2, 110, 2, 14, 2, 449390, 2, 14, 2,
    110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 7022,
    2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14,
    2, 56174, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2,
    110, 2, 14, 2, 7022, 2, 14, 2, 110, 2, 14, 2, 878,
    2, 14, 2, 110, 2, 14, 2
};

static const std::array<int32_t, 256> deltaZ = {
    4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4,
    28, 4, 14044, 4, 28, 4, 220, 4, 28, 4, 1756, 4,
    28, 4, 220, 4, 28, 4, 112348, 4, 28, 4, 220, 4,
    28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 14044, 4,
    28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28,
    4, 898780, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28,
    4, 220, 4, 28, 4, 14044, 4, 28, 4, 220, 4, 28, 4,
    1756, 4, 28, 4, 220, 4, 28, 4, 112348, 4, 28, 4,
    220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 14044,
    4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4,
    28, 4, 7190236, 4, 28, 4, 220, 4, 28, 4, 1756, 4,
    28, 4, 220, 4, 28, 4, 14044, 4, 28, 4, 220, 4, 28,
    4, 1756, 4, 28, 4, 220, 4, 28, 4, 112348, 4, 28, 4,
    220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 14044,
    4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28,
    4, 898780, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4,
    220, 4, 28, 4, 14044, 4, 28, 4, 220, 4, 28, 4, 1756,
    4, 28, 4, 220, 4, 28, 4, 112348, 4, 28, 4, 220, 4, 28,
    4, 1756, 4, 28, 4, 220, 4, 28, 4, 14044, 4, 28, 4, 220,
    4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4
};

constexpr inline auto NegXDelta(const size_t x) {
    return -deltaX[x - 1];
}

constexpr inline auto PosXDelta(const size_t x) {
    return deltaX[x];
}

constexpr inline auto NegYDelta(const size_t y) {
    return -deltaY[y - 1];
}

constexpr inline auto PosYDelta(const size_t y) {
    return deltaY[y];
}

constexpr inline auto NegZDelta(const size_t z) {
    return -deltaZ[z - 1];
}

constexpr inline auto PosZDelta(const size_t z) {
    return deltaZ[z];
}

//void ChunkMeshingSystem::setBlockLightingData(const uint32_t& x, const uint32_t& y, const uint32_t& z, std::array<BlockType, 27>& neighbor_blocks, std::array<float, 27>& neighbor_shades) const {
//	size_t idx = 0;
//	if (x > 0 && y > 0 && z > 0 && x < CHUNK_SIZE && y < CHUNK_SIZE_Y && z < CHUNK_SIZE) {
//		for (int dx = -1; dx <= 1; ++dx) {
//			for (int dy = -1; dy <= 1; ++dy) {
//				for (int dz = -1; dz <= 1; ++dz) {
//					neighbor_blocks[idx] = terrainBlocks[GetBlockIndex(x + dx, y + dy, z + dz)].GetType();
//					neighbor_shades[idx] = 0.0f;
//					// If current block is the topmost in this column, do the following.
//					if (terrainBlocks[GetBlockIndex(x + dx, y + dy + 1, z + dz)].Active()) {
//						for (int offset_y = 0; offset_y < 8; ++offset_y) {
//							if (terrainBlocks[GetBlockIndex(x + dx, y + dy + offset_y, z + dz)].Opaque()) {
//								neighbor_shades[idx] = 1.0f - (static_cast<float>(offset_y) * 0.125f);
//								break;
//							}
//						}
//					}
//				}
//			}
//			idx++;
//		}
//	}
//}

void ChunkMeshingSystem::createBlockFace(const BlockFace & face, const size_t & uv_idx, const glm::vec3 & pos, ChunkMeshComponent& cmp) {
    ChunkMeshComponent::vertex_t v0, v1, v2, v3;

    getFaceVertices(face, v0, v1, v2, v3, uv_idx);

    v0.Position += pos;
    v1.Position += pos;
    v2.Position += pos;
    v3.Position += pos;

    uint32_t i0, i1, i2, i3;
    i0 = cmp.addVertex(std::move(v0));
    i1 = cmp.addVertex(std::move(v1));
    i2 = cmp.addVertex(std::move(v2));
    i3 = cmp.addVertex(std::move(v3));

    cmp.Indices.insert(std::end(cmp.Indices), { i0, i1, i2 });
    cmp.Indices.insert(std::end(cmp.Indices), { i0, i2, i3 });

}

void ChunkMeshingSystem::GenerateMesh(const ecs::entity_t ent, ChunkMeshComponent & mesh) {
    using namespace ecs;
    // Iterate through every block in this chunk one-by-one to decide how/if to render it.
    auto& registry = default_registry_t::get_registry();

    auto& chunk_component = registry.get<ChunkComponent>(ent);

    auto block_alive = [&chunk_component,&registry](const uint32_t i, const uint32_t j, const uint32_t k) {
        return registry.alive(chunk_component.Blocks[morton_encode(i, j, k)]);
    };


    for (uint32_t k = 0; k < CHUNK_SIZE; ++k) {
        for (uint32_t j = 0; j < CHUNK_SIZE_Y; ++j) {
            for (uint32_t i = 0; i < CHUNK_SIZE; ++i) {
                // Get index of the current block we're at.
                uint32_t idx = morton_encode(i, j, k);
                entity_t block = chunk_component.Blocks[idx];

                if (!registry.alive(chunk_component.Blocks[idx])) {
                    continue;
                }
                else {
                    auto& curr_block = registry.get<BlockComponent>(chunk_component.Blocks[idx]);

                    bool xNeg = true; // left
                    if (i > 0) {
                        if (!terrainBlocks[GetBlockIndex(i - 1, j, k)].Active()) {
                            xNeg = false;
                        }
                    }

                    bool xPos = true; // right
                    if (i < CHUNK_SIZE - 1) {
                        if (!terrainBlocks[GetBlockIndex(i + 1, j, k)].Active()) {
                            xPos = false;
                        }
                    }

                    bool yPos = true; // bottom
                    if (j > 0) {
                        if (!terrainBlocks[GetBlockIndex(i, j - 1, k)].Active()) {
                            yPos = false;
                        }
                    }

                    bool yNeg = true; // top
                    if (j < CHUNK_SIZE_Y - 1) {
                        //std::cerr << GetBlockIndex(i, j - 1, k);
                        if (!terrainBlocks[GetBlockIndex(i, j + 1, k)].Active()) {
                            yNeg = false;
                        }
                    }

                    bool zNeg = true; // back
                    if (k < CHUNK_SIZE - 1) {
                        if (!terrainBlocks[GetBlockIndex(i, j, k + 1)].Active()) {
                            zNeg = false;
                        }
                    }

                    bool zPos = true; // front
                    if (k > 0) {
                        if (!terrainBlocks[GetBlockIndex(i, j, k - 1)].Active()) {
                            zPos = false;
                        }
                    }

                    createCube(i, j, k, zNeg, xPos, yNeg, xNeg, yPos, zPos, uv_type);

                }
            }
        }
    }

}

void ChunkMeshingSystem::getFaceVertices(const BlockFace& face, ChunkMeshComponent::vertex_t& v0, ChunkMeshComponent::vertex_t& v1, ChunkMeshComponent::vertex_t& v2, ChunkMeshComponent::vertex_t& v3,
    const size_t& texture_idx) {
    switch (face) {
    case BlockFace::BOTTOM:
        v0.Position = base_vertices[5];
        v1.Position = base_vertices[4];
        v2.Position = base_vertices[1];
        v3.Position = base_vertices[0];
        break;
    case BlockFace::TOP:
        v0.Position = base_vertices[3];
        v1.Position = base_vertices[2];
        v2.Position = base_vertices[7];
        v3.Position = base_vertices[6];
        break;
    case BlockFace::BACK:
        v0.Position = base_vertices[4];
        v1.Position = base_vertices[5];
        v2.Position = base_vertices[6];
        v3.Position = base_vertices[7];
        break;
    case BlockFace::FRONT:
        v0.Position = base_vertices[0];
        v1.Position = base_vertices[1];
        v2.Position = base_vertices[2];
        v3.Position = base_vertices[3];
        break;
    case BlockFace::LEFT:
        v0.Position = base_vertices[5];
        v1.Position = base_vertices[0];
        v2.Position = base_vertices[3];
        v3.Position = base_vertices[6];
        break;
    case BlockFace::RIGHT:
        v0.Position = base_vertices[1];
        v1.Position = base_vertices[4];
        v2.Position = base_vertices[7];
        v3.Position = base_vertices[2];
        break;
    default:
        throw std::runtime_error("Tried to create face with invalid face type enum value");
    }

    v0.Normal = v1.Normal = v2.Normal = v3.Normal = normals[static_cast<size_t>(face)];
    v0.UV = glm::ivec3(0, 0, textures[texture_idx][static_cast<size_t>(face)]);
    v1.UV = glm::ivec3(1, 0, textures[texture_idx][static_cast<size_t>(face)]);
    v2.UV = glm::ivec3(1, 1, textures[texture_idx][static_cast<size_t>(face)]);
    v3.UV = glm::ivec3(0, 1, textures[texture_idx][static_cast<size_t>(face)]);
}

void ChunkMeshingSystem::createCube(const size_t& x, const size_t& y, const size_t& z, const bool& front_face, const bool& right_face, const bool& top_face, const bool& left_face,
	const bool& bottom_face, const bool& back_face, const size_t& uv_idx, ChunkMeshComponent& cmp) {

	// Following method for generating lighting data from:
	// https://github.com/fogleman/Craft/blob/master/src/main.c#L1077
	/*std::array<BlockType, 27> neighbors;
	std::array<float, 27> shades;
	setBlockLightingData(x, y, z, neighbors, shades);*/

	// Builds a side of a cube
	glm::vec3 block_pos = glm::vec3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));

	if (!front_face) {
		createBlockFace(BlockFace::FRONT, uv_idx, block_pos, cmp);
	}

	if (!right_face) {
		createBlockFace(BlockFace::RIGHT, uv_idx, block_pos, cmp);
	}

	if (!top_face) {
		createBlockFace(BlockFace::TOP, uv_idx, block_pos, cmp);
	}

	if (!left_face) {
		createBlockFace(BlockFace::LEFT, uv_idx, block_pos, cmp);
	}

	if (!bottom_face) {
		createBlockFace(BlockFace::BOTTOM, uv_idx, block_pos, cmp);
	}

	if (!back_face) {
		createBlockFace(BlockFace::BACK, uv_idx, block_pos, cmp);
	}

}

uint32_t ChunkMeshComponent::addVertex(vertex_t && v) {
    Vertices.emplace_back(v);
    return Vertices.size() - 1;
}
