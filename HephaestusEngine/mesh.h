#ifndef MESH_H

#define MESH_H
// STD libs
#include "stdafx.h"
#include <stdint.h>
#include <assert.h>
// external libs
#define GLM_FORCE_INLINE
#include <glm/glm.hpp>

/*
	Data: Vertex (vec3 pos, vec3 norm, *datatype data), meshVerts (vector of vertices), meshIndices (vector of unit32's), 
	compressed_Vertex(uvec3 pos, uint16_t norm, *datatype data)
	Methods: too numerous to list. Basic, comments on functionality given where not self-explanatory
		> Note: included mesh compression and decompression algorithms are not mine. The position functions
			are of the sort I had previously written, but the normal ones are far beyond me. Polyvox is the propietor of those! 
	Generalized mesh class, created for hopefully allowing meshes besides blocks down the road. Currently there is a specialized sub-class CubeMesh.
	This class is as basic as it can be, so it can easily imported elsewhere are used as a sort of Abstract Base Class.

	Templates are commonly used so that datatypes can be set as class or even program-wide variables. This makes changing the datatype due to 
	bugs or general changes far easier than it would otherwise be.
*/


// Simple template for a vertex. datatype exists to allow us to add data as needed later (texture, color, AO maths, etc)
struct vertex {
	glm::highp_vec3 position;
	glm::highp_vec3 normal;
	glm::highp_vec2 uv;
};

// This more complex template is a highly compressed vertex format. A decoder is implemented in this class,
// but it would be wiser to decode on the GPU. Smaller mesh data helps program and rendering speed in
// general, but additional benefit comes from aligning the data in 4-byte chunks (or multiples thereof)
struct compressed_vertex {
	// Each position component is stored using fixed-point encoding (8.8). The vector has a size of 12bytes
	glm::highp_ivec3 position;
	// 4 byte padding data, enable or disable as needed
	//uint8_t _pad[4];
	// The normal is encoded as a 16 bit unsigned int using oct16 encoding found here http://jcgt.org/published/0003/02/01/
	uint16_t normal;
};

// Define index type here, and change it here too
typedef uint32_t index_t;
typedef vertex vertType;

class Mesh {
public:
	Mesh();
	~Mesh();

	index_t getNumVerts(void) const;
	const vertType& getVertexData(index_t index); // get data from one vertex
	const vertType* getRawVertexData(void) const; // Get entire list of vertices in this mesh

	size_t getNumIndices(void) const;
	index_t getIndex(index_t index) const; // get the index at index
	const index_t* getRawIndexData(void) const; // Get the entire list of indices used in this mesh

	const glm::mediump_vec3& getoffset(void) const; // this is always going to be an integer, so we can use a medium_precision vector
	void setOffset(const glm::mediump_vec3& offset); // set offset of mesh (usually relative to world or chunk coords)

	index_t addVert(const vertType& vertex); // add a vertex and return its index
	void addTriangle(index_t index0, index_t index1, index_t index2); // add the three indices needed to define a triangle
	void addUV(int t, int s);

	bool clear(void); // delete the mesh
	bool isEmpty(void) const; // return true if meshVerts or meshIndices is empty

	// Mesh encoding/decoding functions for using the comrpessed vertex format
	glm::vec3 decodePosition(const glm::mediump_ivec3& encodedPos);
	glm::mediump_ivec3 encodePosition(const glm::vec3& pos);
	uint16_t encodeNormal(const glm::vec3& normal);
	glm::vec3 decodeNormal(const uint16_t encodedNormal);
	std::vector<index_t> meshIndices;
	std::vector<vertType> meshVerts;
	glm::mediump_vec3 meshOffset;
	
};
#endif // !MESH_H


inline Mesh::Mesh(){
}

inline Mesh::~Mesh(){
}


inline index_t Mesh::getNumVerts(void) const{
	return static_cast<index_t>(meshVerts.size());
}


inline const vertType& Mesh::getVertexData(index_t index){
	return meshVerts[index];
}


inline const vertType* Mesh::getRawVertexData(void) const{
	return meshVerts.data();
}


inline size_t Mesh::getNumIndices(void) const{
	return meshIndices.size();
}


inline index_t Mesh::getIndex(index_t index) const{
	return meshIndices[index];
}


inline const index_t* Mesh::getRawIndexData(void) const{
	return meshIndices.data();
}


inline const glm::mediump_vec3& Mesh::getoffset(void) const{
	return meshOffset;
}


inline void Mesh::setOffset(const glm::mediump_vec3 & offset){
	meshOffset = offset;
}


inline index_t Mesh::addVert(const vertType & vertex){
	meshVerts.push_back(vertex);
	return meshVerts.size() - 1;
}


inline void Mesh::addTriangle(index_t index0, index_t index1, index_t index2){
	meshIndices.push_back(index0); meshIndices.push_back(index1); meshIndices.push_back(index2);
}


inline bool Mesh::clear(void) {
	return((getNumVerts() == 0 ) || (getNumIndices() == 0));
}


// These functions are from the excellent polyvox library, specifically here: 
// https://bitbucket.org/volumesoffun/polyvox/src/9a71004b1e72d6cf92c41da8995e21b652e6b836/include/PolyVox/MarchingCubesSurfaceExtractor.inl
inline glm::vec3 Mesh::decodePosition(const glm::mediump_ivec3& encodedPos) {
	glm::vec3 result(encodedPos.x, encodedPos.y, encodedPos.z);
	result *= (1.0f / 2560.0f); 
	return result; // decoded vertex
}

inline glm::mediump_ivec3 Mesh::encodePosition(const glm::vec3& pos) {
	glm::vec3 temp = pos;
	temp *= (2560.0f);
	glm::mediump_vec3 result(temp.x, temp.y, temp.z);
	return result;
}

inline uint16_t Mesh::encodeNormal(const glm::vec3& normal) {
	float vx = normal.x;
	float vy = normal.y;
	float vz = normal.z;

	// Project the sphere onto the octahedron, and then onto the xy plane					
	float px = vx * (1.0f / (std::abs(vx) + std::abs(vy) + std::abs(vz)));
	float py = vy * (1.0f / (std::abs(vx) + std::abs(vy) + std::abs(vz)));

	// Reflect the folds of the lower hemisphere over the diagonals.
	if (vz <= 0.0f)
	{
		float refx = ((1.0f - std::abs(py)) * (px >= 0.0f ? +1.0f : -1.0f));
		float refy = ((1.0f - std::abs(px)) * (py >= 0.0f ? +1.0f : -1.0f));
		px = refx;
		py = refy;
	}

	// The next part was not given in the paper. We map our two
	// floats into two bytes and store them in a single uint16_t

	// Move from range [-1.0f, 1.0f] to [0.0f, 255.0f]
	px = (px + 1.0f) * 127.5f;
	py = (py + 1.0f) * 127.5f;

	// Convert to uints
	uint16_t resultX = static_cast<uint16_t>(px + 0.5f);
	uint16_t resultY = static_cast<uint16_t>(py + 0.5f);

	// Make sure only the lower bits are set. Probably
	// not necessary but we're just being careful really.
	resultX &= 0xFF;
	resultY &= 0xFF;

	// Contatenate the bytes and return the result.
	return (resultX << 8) | resultY;
}

inline glm::vec3 Mesh::decodeNormal(const uint16_t encodedNormal) {
	uint16_t ux = (encodedNormal >> 8) & 0xFF;
	uint16_t uy = (encodedNormal) & 0xFF;

	// Convert to floats in the range [-1.0f, +1.0f].
	float ex = ux / 127.5f - 1.0f;
	float ey = uy / 127.5f - 1.0f;

	// Reconstruct the origninal vector. This is a C++ implementation
	// of Listing 2 of http://jcgt.org/published/0003/02/01/
	float vx = ex;
	float vy = ey;
	float vz = 1.0f - std::abs(ex) - std::abs(ey);

	if (vz < 0.0f)
	{
		float refX = ((1.0f - std::abs(vy)) * (vx >= 0.0f ? +1.0f : -1.0f));
		float refY = ((1.0f - std::abs(vx)) * (vy >= 0.0f ? +1.0f : -1.0f));
		vx = refX;
		vy = refY;
	}

	// Normalise and return the result.
	glm::vec3 v(vx, vy, vz);
	v = glm::normalize(v); // normalization can always help compress - we can encode as byte 
	// and specify to GL that things are as such
	return v;
}



