#ifndef MESH_H

#define MESH_H
// STD libs
#include "stdafx.h"
#include <stdint.h>
#include <assert.h>
// external libs
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
template<typename _Datatype>
struct vertex {
	typedef _Datatype datatype;
	glm::vec3 position;
	glm::vec3 normal;
	datatype data;
};
// This more complex template is a highly compressed vertex format. A decoder is implemented in this class,
// but it would be wiser to decode on the GPU. Smaller mesh data helps program and rendering speed in
// general, but additional benefit comes from aligning the data in 4-byte chunks (or multiples thereof)
template<typename _Datatype>
struct compressed_vertex {
	typedef _Datatype datatype;
	// Each position component is stored using fixed-point encoding (8.8)
	glm::mediump_uvec3 position;
	// The normal is encoded as a 16 bit unsigned int using oct16 encoding found here http://jcgt.org/published/0003/02/01/
	uint16_t normal;
	// extra data as needed. If so, consider alignment still!
	datatype data;
};

// Define index type here, and change it here too
typedef uint32_t index_t;
template<typename _vertType, typename _indexType = index_t>

class Mesh {
	typedef _indexType indexType;
	typedef _vertType vertType;
public:
	Mesh();
	~Mesh();

	indexType getNumVerts(void) const;
	const vertType& getVertexData(indexType index); // get data from one vertex
	const vertType* getRawVertexData(void) const; // Get entire list of vertices in this mesh

	size_t getNumIndices(void) const;
	indexType getIndex(_indexType index) const; // get the index at index
	const indexType* getRawIndexData(void) const; // Get the entire list of indices used in this mesh

	const glm::mediump_vec3& getoffset(void) const; // this is always going to be an integer, so we can use a medium_precision vector
	void setOffset(const glm::mediump_vec3& offset); // set offset of mesh (usually relative to world or chunk coords)

	indexType addVert(const vertType& vertex); // add a vertex and return its index
	void addTriangle(indexType index0, indexType index1, indexType index2); // add the three indices needed to define a triangle

	void clear(void); // delete the mesh
	bool isEmpty(void) const; // return true if meshVerts or meshIndices is empty
private:
	std::vector<indexType> meshIndices;
	std::vector<vertType> meshVerts;
	glm::mediump_vec3 meshOffset;
	
};
#endif // !MESH_H

template<typename _vertType, typename _indexType>
inline Mesh<_vertType, _indexType>::Mesh(){
}

template<typename _vertType, typename _indexType>
inline Mesh<_vertType, _indexType>::~Mesh(){
}

template<typename _vertType, typename _indexType>
inline indexType Mesh<_vertType, _indexType>::getNumVerts(void) const{
	return static_cast<indexType>(meshVerts.size());
}

template<typename _vertType, typename _indexType>
inline const vertType & Mesh<_vertType, _indexType>::getVertexData(indexType index){
	return meshVertices[index];
}

template<typename _vertType, typename _indexType>
inline const vertType * Mesh<_vertType, _indexType>::getRawVertexData(void) const{
	return meshVertices.data();
}

template<typename _vertType, typename _indexType>
inline size_t Mesh<_vertType, _indexType>::getNumIndices(void) const{
	return meshIndices.size();
}

template<typename _vertType, typename _indexType>
inline indexType Mesh<_vertType, _indexType>::getIndex(_indexType index) const{
	return meshIndices[index];
}

template<typename _vertType, typename _indexType>
inline const indexType * Mesh<_vertType, _indexType>::getRawIndexData(void) const{
	return meshIndices.data();
}

template<typename _vertType, typename _indexType>
inline const glm::mediump_vec3 & Mesh<_vertType, _indexType>::getoffset(void) const{
	return meshOffset;
}

template<typename _vertType, typename _indexType>
inline void Mesh<_vertType, _indexType>::setOffset(const glm::mediump_vec3 & offset){
	meshOffset = offset;
}

template<typename _vertType, typename _indexType>
inline indexType Mesh<_vertType, _indexType>::addVert(const vertType & vertex){
	meshVerts.push_back(vertex);
	return meshVerts.size() - 1;
}

template<typename _vertType, typename _indexType>
inline void Mesh<_vertType, _indexType>::addTriangle(indexType index0, indexType index1, indexType index2){
	std::ASSERT(index0 < meshVerts.size(), "index0 out of range or invalid");
	std::ASSERT(index1 < meshVerts.size(), "index1 out of range or invalid");
	std::ASSERT(index2 < meshVerts.size(), "index2 out of range or invalid");

	meshIndices.push_back(index0); meshIndices.push_back(index1); meshIndices.push_back(index2);
}

template<typename _vertType, typename _indexType>
inline void Mesh<_vertType, _indexType>::clear(void) {
	return(getNumVerts() == 0 )|| (getNumIndices() == 0));
}


// These functions are from the excellent polyvox library, specifically here: 
// https://bitbucket.org/volumesoffun/polyvox/src/9a71004b1e72d6cf92c41da8995e21b652e6b836/include/PolyVox/MarchingCubesSurfaceExtractor.inl
inline glm::vec3 decodePosition(const glm::mediump_uvec3& encodedPos) {
	glm::vec3 result(encodedPos.x, encodedPos.y, encodedPos.z);
	result *= (1.0f / 256.0f); 
	return result; // decoded vertex
}

inline glm::mediump_uvec3 encodePosition(const glm::vec3& pos) {
	glm::mediump_uvec3 result(pos.x, pos.y, pos.z);
	result *= (256.0f);
	return result;
}

inline uint16_t encodeNormal(const glm::vec3& normal) {
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

inline glm::vec3 decodeNormal(const uint16_t encodedNormal) {
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



