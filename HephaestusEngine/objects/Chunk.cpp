#include "stdafx.h"
#include "Chunk.h"
#include "../util/rle.h"

namespace objects {
	// Face normals. Don't change and can be reused. Yay for cubes!
	static const std::array<glm::ivec3, 6> normals = {
		glm::ivec3(0, 0, 1),   // (front)
		glm::ivec3(1, 0, 0),   // (right)
		glm::ivec3(0, 1, 0),   // (top)
		glm::ivec3(-1, 0, 0),   // (left)
		glm::ivec3(0,-1, 0),   // (bottom)
		glm::ivec3(0, 0,-1),   // (back)
	};

	/*

		Texture indices array:

		Each block will always use the same UV coordinates: recall that texture coordinates
		are usually 2D coordinates, (u, v). The only component that will change is the third component:
		our UV coords are unconventially 3D because our 3rd component is how we select different textures.
		Instead of binding a different texture for each block, we use a texture array. We index through
		this array using this 3rd component, like it was the index into a regular ol array or vector.

		Note that even among blocks there is variation in these indices. This is because some blocks have
		different textures on each face: grass blocks, for example, use the pure grass texture only on
		the upper face, and actually re-use the dirt texture on their bottom face.

	*/

	static constexpr unsigned int textures[256][6] = {
		// Each number corresponds to certain face, and thus certain index into texture array. Given as {front, right, top, left, bottom, back}
		// Loading order:: Grass_Top, Grass_Sides, dirt, sand, stone, bedrock
		{ 0,0,0,0,0,0 }, // Air block
		{ 1,1,0,1,2,1 }, // Grass block
		{ 3,3,3,3,3,3 }, // Sand block
		{ 2,2,2,2,2,2 }, // Dirt block
		{ 4,4,4,4,4,4 }, // Stone block
		{ 5,5,5,5,5,5 }, // Bedrock block
		{ 6,6,6,6,6,6 }, // tall grass
		{ 7,7,7,7,7,7 }, // Coal ore
		{ 8,8,8,8,8,8 }, // Iron ore
		{ 9,9,9,9,9,9 }, // Diamond Ore
	};


	/*

		Struct - AO Lookup

		This is an ambient-occlusion lookup table. Ambient occlusion is used to slightly darken the interior edges and corners of objects,
		giving them a more realistic look and increasing the amount of perceived depth in an image.

		We call this struct once for each block, and calling its constructor with the position of the current block we're getting AO
		values for automatically sets up the lookup table for us

	*/
	struct aoLookup {

		/*
		Old comment used to make this table: first entry is vertex position on the unit cube, second is the offsets of the spots we
		need to check for occlusion values.

		(vertex)   (spots to check)
		0, 0, 0 - (-1, -1, -1), (-1, -1, 0), (0, -1, -1)
		1, 0, 0 - (1, -1, -1), (0, -1, -1), (1, -1, 0)
		0, 1, 0 - (-1, 1, -1), (-1, 1, 0), (0, 1, -1)
		0, 0, 1 - (-1, -1, 1), (-1, -1, 0), (0, -1, 1)
		1, 1, 0 - (1, 1, -1), (0, 1, -1), (1, 1, 0)
		0, 1, 1 - (-1, 1, 1), (-1, 1, 0), (0, 1, 1)
		1, 0, 1 - (1, -1, 1), (1, -1, 0), (0, -1, 1)
		1, 1, 1 - (1, 1, 1), (1, 1, 0), (0, 1, 1)

		*/

		aoLookup(int x, int y, int z) : LUT{
			// Vertex 0, 0, 1
			{ GetBlockIndex(x - 1, y - 1, z + 1), GetBlockIndex(x - 1, y - 1, z), GetBlockIndex(x, y - 1, z + 1), },
			// Vertex 1, 0, 1
			{ GetBlockIndex(x + 1, y - 1, z + 1), GetBlockIndex(x + 1, y - 1, z), GetBlockIndex(x, y - 1, z + 1), },
			// Vertex 1, 1, 1
			{ GetBlockIndex(x + 1, y + 1, z + 1), GetBlockIndex(x + 1, y + 1, z), GetBlockIndex(x, y + 1, z + 1), },
			// Vertex 0, 1, 1
			{ GetBlockIndex(x - 1, y + 1, z + 1), GetBlockIndex(x - 1, y + 1, z), GetBlockIndex(x, y + 1, z + 1), },
			// Vertex 1, 0, 0
			{ GetBlockIndex(x + 1, y - 1, z - 1), GetBlockIndex(x, y - 1, z - 1), GetBlockIndex(x + 1, y - 1, z), },
			// Vertex 0, 0, 0
			{ GetBlockIndex(x - 1, y - 1, z - 1), GetBlockIndex(x - 1, y - 1, z), GetBlockIndex(x, y - 1, z - 1), },
			// Vertex 0, 1, 0
			{ GetBlockIndex(x - 1, y + 1, z - 1), GetBlockIndex(x - 1, y + 1, z), GetBlockIndex(x, y + 1, z - 1), },
			// Vertex 1, 1, 0
			{ GetBlockIndex(x + 1, y + 1, z - 1), GetBlockIndex(x, y + 1, z - 1), GetBlockIndex(x + 1, y + 1, z), },

		} {
			this->X = x;
			this->Y = y;
			this->Z = z;
		}

		// Defaulted destructor.
		~aoLookup() = default;

		// XYZ coords of this lookup object
		int X, Y, Z;

		// Array that makes up this lookup object (i.e this is the LUT itself)
		int LUT[8][3];

	};

	// Build the mesh data for a cube at position XYZ, building the faces specified by each boolean if that boolean is false (false = there isn't another block in this location)
	inline void Chunk::createCube(int x, int y, int z, bool frontFace, bool rightFace, bool topFace, bool leftFace, bool bottomFace, bool backFace, int uv_type) {
		// Use a std::array since the data isn't modified, rather it's used like a template to build the individual points from
		// This setup means that the xyz of a given block is actually the center of the block's mesh
		std::array<glm::vec3, 8> vertices{
		{   glm::vec3(x - 0.50f,y - 0.50f,z + 0.50f), // Point 0, left lower front UV{0,0}
			glm::vec3(x + 0.50f,y - 0.50f,z + 0.50f), // Point 1, right lower front UV{1,0}
			glm::vec3(x + 0.50f,y + 0.50f,z + 0.50f), // Point 2, right upper front UV{1,1}
			glm::vec3(x - 0.50f,y + 0.50f,z + 0.50f), // Point 3, left upper front UV{0,1}
			glm::vec3(x + 0.50f,y - 0.50f,z - 0.50f), // Point 4, right lower rear
			glm::vec3(x - 0.50f,y - 0.50f,z - 0.50f), // Point 5, left lower rear
			glm::vec3(x - 0.50f,y + 0.50f,z - 0.50f), // Point 6, left upper rear
			glm::vec3(x + 0.50f,y + 0.50f,z - 0.50f), // Point 7, right upper rear
		}
		};

		// Gets occlusion value for a vertex, given by "index" in a cube
		// Val sets the occlusion level, and we use it in the fragment shader to decide 
		// how much to darken a vertex. A higher value = more occluded vertex = darker vertex.
		auto AOVal = [this](int index, aoLookup& in)->GLuint {
			int val = 0;
			if (this->terrainBlocks[in.LUT[index][0]] != blockTypes::AIR) {
				val++;
			}
			if (this->terrainBlocks[(in.LUT[index][1])] != blockTypes::AIR) {
				val++;
			}
			if (this->terrainBlocks[in.LUT[index][2]] != blockTypes::AIR) {
				val++;
			}
			return val;
		};

		// Builds a side of a cube
		glm::vec3 blockPosition = glm::vec3(x, y, z);
		auto buildface = [this, uv_type, AOVal, blockPosition, vertices](index_t i00, index_t i01, index_t i02, index_t i03, int norm, int face) {
			// Get points from input indices into pre-built vertex array
			glm::vec3 p0, p1, p2, p3;
			p0 = vertices[i00];
			p1 = vertices[i01];
			p2 = vertices[i02];
			p3 = vertices[i03];

			// We'll need four indices and four vertices for the two tris defining a face.
			index_t i0, i1, i2, i3;
			vertex_t v0, v1, v2, v3;

			// Assign each vertex it's appropriate UV coords based on the blocks type
			glm::vec3 uv0 = glm::ivec3(0.0, 0.0, textures[uv_type][face]);
			glm::vec3 uv1 = glm::ivec3(1.0, 0.0, textures[uv_type][face]);
			glm::vec3 uv3 = glm::ivec3(0.0, 1.0, textures[uv_type][face]);
			glm::vec3 uv2 = glm::ivec3(1.0, 1.0, textures[uv_type][face]);
			v0.UV = uv0;
			v1.UV = uv1;
			v2.UV = uv2;
			v3.UV = uv3;

			// Set the vertex positions.
			v0.Position.xyz = p0;
			v1.Position.xyz = p1;
			v2.Position.xyz = p2;
			v3.Position.xyz = p3;

			// Set vertex normals.
			v0.Normal = normals[norm];
			v1.Normal = normals[norm];
			v2.Normal = normals[norm];
			v3.Normal = normals[norm];

			// Set ambient occlusion values. (if this block isn't at the extrema of a chunk)
			if (blockPosition.x > 0 && blockPosition.y > 0 && blockPosition.z > 0 &&
				blockPosition.x < CHUNK_SIZE && blockPosition.y < CHUNK_SIZE && blockPosition.z < CHUNK_SIZE) {

				// TODO: AO Lookup is broken for certain directions, and along chunk edges. How to fix this?
				aoLookup offsets(static_cast<int>(blockPosition.x), static_cast<int>(blockPosition.y), static_cast<int>(blockPosition.z));
				GLuint ao0, ao1, ao2, ao3;
				ao0 = AOVal(i00, offsets);
				ao1 = AOVal(i01, offsets);
				ao2 = AOVal(i02, offsets);
				ao3 = AOVal(i03, offsets);
				v0.ao = ao0;
				v1.ao = ao1;
				v2.ao = ao2;
				v3.ao = ao3;

				// This next conditional is due to how triangle winding in our cubes works:
				// If we don't do this, we get artifacting in the AO values that looks angular and unnatural.
				if (v0.ao + v1.ao > v2.ao + v3.ao) {
					// Add the verts to the Mesh's vertex container. Returns index to added vert.
					i0 = this->mesh.AddVert(v3);
					i1 = this->mesh.AddVert(v2);
					i2 = this->mesh.AddVert(v1);
					i3 = this->mesh.AddVert(v0);
					// Add the triangles to the mesh, via indices
					this->mesh.AddTriangle(i0, i1, i2); // Needs UVs {0,0}{1,0}{0,1}
					this->mesh.AddTriangle(i0, i2, i3); // Needs UVs {1,0}{0,1}{1,1}
				}
				else {
					// Add the verts to the Mesh's vertex container. Returns index to added vert.
					i0 = this->mesh.AddVert(v0);
					i1 = this->mesh.AddVert(v1);
					i2 = this->mesh.AddVert(v2);
					i3 = this->mesh.AddVert(v3);
					// Add the triangles to the mesh, via indices
					this->mesh.AddTriangle(i0, i1, i2); // Needs UVs {0,0}{1,0}{0,1}
					this->mesh.AddTriangle(i0, i2, i3); // Needs UVs {1,0}{0,1}{1,1}
				}
			}
			// TODO: Note that this is why AO values are broken on chunk edges, since we can't look up adjacent chunks at the moment.
			else {
				v0.ao = static_cast<GLuint>(0);
				v1.ao = static_cast<GLuint>(0);
				v2.ao = static_cast<GLuint>(0);
				v3.ao = static_cast<GLuint>(0);
				// Add the verts to the Mesh's vertex container. Returns index to added vert.
				i0 = this->mesh.AddVert(v0);
				i1 = this->mesh.AddVert(v1);
				i2 = this->mesh.AddVert(v2);
				i3 = this->mesh.AddVert(v3);
				// Add the triangles to the mesh, via indices
				this->mesh.AddTriangle(i0, i1, i2); // Needs UVs {0,0}{1,0}{0,1}
				this->mesh.AddTriangle(i0, i2, i3); // Needs UVs {1,0}{0,1}{1,1}
			}
		};

		// The following statements build a face based on the boolean value given to this method,
		// using the lambda (inline) function above.
		if (frontFace == false) {
			buildface(0, 1, 2, 3, 0, 0); // Using Points 0, 1, 2, 3 and Normal 0
		}
		if (rightFace == false) {
			buildface(1, 4, 7, 2, 1, 1); // Using Points 1, 4, 7, 2 and Normal 1
		}
		if (topFace == false) {
			buildface(3, 2, 7, 6, 2, 2); // Using Points 3, 2, 7, 6 and Normal 2
		}

		if (leftFace == false) {
			buildface(5, 0, 3, 6, 3, 3); // Using Points 5, 0, 3, 6 and Normal 3
		}

		if (bottomFace == false) {
			buildface(5, 4, 1, 0, 4, 4); // Using Points 5, 4, 1, 0 and Normal 4
		}

		if (backFace == false) {
			buildface(4, 5, 6, 7, 5, 5); // Using Points 4, 5, 6, 7 and Normal 5
		}

	}

	Chunk::Chunk(glm::ivec2 gridpos) {
		// Set grid position.
		GridPosition = gridpos;

		// Reserve space in our textures container.
		std::size_t totalBlocks = CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE_Y;
		terrainBlocks.assign(totalBlocks, blockTypes::AIR);

		// The gridpos is simply "normalized" world coords to be integral values.
		// The actual position in the world is calculated now - we use this later to offset the chunk using a model matrix
		glm::vec3 float_position;
		float_position.x = this->GridPosition.x * (static_cast<float>(CHUNK_SIZE) / 2.0f);
		float_position.y = 0.0f;
		float_position.z = this->GridPosition.y * (static_cast<float>(CHUNK_SIZE) / 2.0f);

		// Set the updated positions in the mesh (most important), and this chunk (good for reference)
		mesh.Position = float_position;
		Position = float_position;

		// Now use the floating point position to build the model matrix for this chunk, so that it renders in the correct location.
		//mesh.Model = glm::translate(glm::mat4(1.0f), this->Position);

	}

	glm::vec3 Chunk::GetPosFromGrid(glm::ivec2 gridpos) {
		glm::vec3 res;
		res.x = this->GridPosition.x * ((CHUNK_SIZE / 2.0f));
		res.y = static_cast<float>(CHUNK_SIZE_Y) / 2.0f;
		res.z = this->GridPosition.y * (CHUNK_SIZE / 2.0f);
		return res;
	}

	void Chunk::BuildTerrain(TerrainGenerator & gen, int terraintype) {
		for (int x = 0; x < CHUNK_SIZE; ++x) {
			for (int z = 0; z < CHUNK_SIZE; ++z) {

				// Recall that XZ is our left/right forward/back, so set every single block at Y = 0 and 
				// with any XZ value to be bedrock, defining the base layer of our world. 
				this->terrainBlocks[GetBlockIndex(x, 0, z)] = blockTypes::BEDROCK;

				/*

				Following terrain generation method is fast but primitive. We query the noise generator for a value,
				and use that value as a height value. We iterate from above the bedrock layer (Y=1) to whatever this
				height value is.

				*/

				// If we're using the FBM generator, use this branch.
				if (terraintype == 0) {
					for (int y = 1; y < static_cast<int>(gen.SimplexFBM(static_cast<int>(Position.x) + x, static_cast<int>(Position.z) + z)); ++y) {
						// Since we start at y = 1 and iterate up the column progressively
						// set the majority of blocks to be stone, blocks 3 above stone to be grass,
						// blocks 2 above stone to be dirt, blocks 1 above to be dirt to form some basic terrain
						int currentIndex = GetBlockIndex(x, y, z);
						this->terrainBlocks[GetBlockIndex(x, y - 1, z)] = blockTypes::STONE;
						this->terrainBlocks[currentIndex] = blockTypes::DIRT;
						this->terrainBlocks[GetBlockIndex(x, y + 1, z)] = blockTypes::DIRT;
						this->terrainBlocks[GetBlockIndex(x, y + 2, z)] = blockTypes::GRASS;
					}
				}

				// Billow generator.
				if (terraintype == 1) {
					for (int y = 1; y < static_cast<int>(gen.SimplexBillow(static_cast<int>(Position.x) + x, static_cast<int>(Position.z) + z)); ++y) {
						// Since we start at y = 1 and iterate up the column progressively
						// set the majority of blocks to be stone, blocks 3 above stone to be grass,
						// blocks 2 above stone to be dirt, blocks 1 above to be dirt to form some basic terrain
						int currentIndex = GetBlockIndex(x, y, z);
						this->terrainBlocks[GetBlockIndex(x, y - 1, z)] = blockTypes::STONE;
						this->terrainBlocks[currentIndex] = blockTypes::DIRT;
						this->terrainBlocks[GetBlockIndex(x, y + 1, z)] = blockTypes::DIRT;
						this->terrainBlocks[GetBlockIndex(x, y + 2, z)] = blockTypes::GRASS;
					}
				}

				// Ridged generator.
				if (terraintype == 2) {
					for (int y = 1; y < static_cast<int>(gen.SimplexRidged(static_cast<int>(Position.x) + x, static_cast<int>(Position.z) + z)); ++y) {
						// Since we start at y = 1 and iterate up the column progressively
						// set the majority of blocks to be stone, blocks 3 above stone to be grass,
						// blocks 2 above stone to be dirt, blocks 1 above to be dirt to form some basic terrain
						int currentIndex = GetBlockIndex(x, y, z);
						this->terrainBlocks[GetBlockIndex(x, y - 1, z)] = blockTypes::STONE;
						this->terrainBlocks[currentIndex] = blockTypes::DIRT;
						this->terrainBlocks[GetBlockIndex(x, y + 1, z)] = blockTypes::DIRT;
						this->terrainBlocks[GetBlockIndex(x, y + 2, z)] = blockTypes::GRASS;
					}
				}
			}
		}
	}

	void Chunk::BuildMesh() {
		// Default block adjacency value assumes true
		bool def = true;

		// Reserve some space in our containers, hopefully cutting down on costly re-allocations.
		mesh.Indices.reserve(60000);
		mesh.Vertices.reserve(80000);

		// Iterate through every block in this chunk one-by-one to decide how/if to render it.
		for (int j = 0; j < CHUNK_SIZE_Y - 1; j++) {
			for (int i = 0; i < CHUNK_SIZE - 1; i++) {
				for (int k = 0; k < CHUNK_SIZE - 1; k++) {
					// Get index of the current block we're at.
					int currBlock = GetBlockIndex(i, j, k);

					// If the current block is an air block, we don't need to worry about meshing+rendering it.
					if (this->terrainBlocks[currBlock] == blockTypes::AIR) {
						continue;
					}
					// Current block isn't air, lets look at all its adjacent blocks to find out what we need to do next.
					else {
						// The uv_type is simpyly the value of the block at the given point (grabbed from the enum)
						// This is used to index into the texture array, so each block gets the right textures and UVs
						int uv_type;
						uv_type = terrainBlocks[currBlock];

						// If we are primitively culling invisible faces, run this system
						// Primitive culling merely means that we don't render faces we can't see: this 
						// cuts rendered elements by easily 85-95% compared to no culling at all. 
						if (SIMPLE_CULLING_GLOBAL == true) {

							// If a face is visible, set that face's value to be false
							bool xNeg = def; // left
							if (i > 0) {
								if (this->terrainBlocks[GetBlockIndex(i - 1, j, k)] == blockTypes::AIR) {
									xNeg = false;
								}
							}

							bool xPos = def; // right
							if (i < CHUNK_SIZE - 1) {
								if (this->terrainBlocks[GetBlockIndex(i + 1, j, k)] == blockTypes::AIR) {
									xPos = false;
								}
							}

							bool yPos = def; // bottom
							if (j > 0) {
								if (this->terrainBlocks[GetBlockIndex(i, j - 1, k)] == blockTypes::AIR) {
									yPos = false;
								}
							}

							bool yNeg = def; // top
							if (j < CHUNK_SIZE_Y - 1) {
								//std::cerr << GetBlockIndex(i, j - 1, k);
								if (this->terrainBlocks[GetBlockIndex(i, j + 1, k)] == blockTypes::AIR) {
									yNeg = false;
								}
							}

							bool zNeg = def; // back
							if (k < CHUNK_SIZE - 1) {
								if (this->terrainBlocks[GetBlockIndex(i, j, k + 1)] == blockTypes::AIR) {
									zNeg = false;
								}
							}

							bool zPos = def; // front
							if (k > 0) {
								if (this->terrainBlocks[GetBlockIndex(i, j, k - 1)] == blockTypes::AIR) {
									zPos = false;
								}
							}

							// Create a cube at i,j,k with certain faces rendered
							// Each false value specifies a face that should be visible, and thus should be rendered
							// At the end, we include the type of block and thus what texture it needs
							this->createCube(i, j, k, zNeg, xPos, yNeg, xNeg, yPos, zPos, uv_type);
						}
						// If we're not doing primitive culling, render every non-air block in the volume regardless of visibility
						else if (SIMPLE_CULLING_GLOBAL == false) {
							this->createCube(i, j, k, false, false, false, false, false, false, uv_type);
						}
					}
				}
			}
		}
		mesh.Indices.shrink_to_fit();
		mesh.Vertices.shrink_to_fit();
	}

	void Chunk::EncodeBlocks() {
		encodedBlocks.reserve(this->terrainBlocks.size());
		encodedBlocks = encode(this->terrainBlocks);
		encodedBlocks.shrink_to_fit();
	}

	void Chunk::clear() {
		// Mesh clear method clears data and calls shrink_to_fit()
		mesh.Clear();
		// First call clear to empty containers
		terrainBlocks.clear();
		encodedBlocks.clear();
		// Then call shrink to fit to actually free up memory.
		terrainBlocks.shrink_to_fit();
		encodedBlocks.shrink_to_fit();
	}

}