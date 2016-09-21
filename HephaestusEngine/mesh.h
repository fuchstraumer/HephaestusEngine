#ifndef MESH_H

#define MESH_H
// STD libs
#include "stdafx.h"
#include <stdint.h>
#include <assert.h>
// external libs
#include <glm/glm.hpp>

/*
	Data: Vertex (vec3 pos, vec3 norm, *datatype data), meshVerts (vector of vertices), meshIndices (vector of unit32's)
	Methods: too numerous to list. Basic, comments on functionality given where not self-explanatory
	Generalized mesh class, created for hopefully allowing meshes besides blocks down the road. Currently there is a specialized sub-class CubeMesh.
	This class is as basic as it can be, so it can easily imported elsewhere are used as a sort of Abstract Base Class.
*/

// Simple template for a vertex. datatype exists to allow us to add data as needed later (texture, color, AO maths, etc)
template<typename _Datatype>
struct vertex {
	typedef _Datatype datatype;
	glm::vec3 position;
	glm::vec3 normal;
	datatype data;
};

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



