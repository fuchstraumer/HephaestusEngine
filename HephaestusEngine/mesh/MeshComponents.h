#pragma once
#ifndef MESH_COMPONENTS_H
#define MESH_COMPONENTS_H
#include "stdafx.h"

/*
	MESH_COMPONENTS_H

	This header contains definitions and implementations for the Mesh
	components. This includes:

	vertex_t - vertex type
	index_t - index type
	tri_t - triangle type
	face_t - face type
	EdgeLookup - templated version of an unordered map, 
	using a custom hashing function. Stores pairs of indices
	that form edges

*/

// Index type
using index_t = std::uint32_t;

// Vertex struct
struct vertex_t {
public:
	vertex_t(glm::vec3 pos, glm::vec3 norm = glm::vec3(0.0f), glm::vec3 uv = glm::vec3(0.0f), glm::vec2 lighting = glm::vec2(0.0f)) : Position(pos),
	Normal(norm), UV(uv), Lighting(lighting) {}

	vertex_t() : Position(0.0f), Normal(0.0f), UV(0.0f), Lighting(0.0f) { }
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 UV;
	// Holds two variables used to decide how to shade this vertex,
	// first variable relates explicitly to ambient occlusion
	glm::vec2 Lighting;
};


// Triangle struct - three vertices that make up the base drawn type
template<typename index_type = index_t>
struct triangle_t {

	// Create a triangle using the three vertex indices giving.
	triangle_t(index_type const &i0, index_type const &i1, index_type const &i2) {
		this->i0 = i0;
		this->i1 = i1;
		this->i2 = i2;
		MakeEdges();
	}

	// Default empty constructor
	triangle_t() { }

	// Vertex indices for this triangle
	index_type i0, i1, i2;

};

#endif // MESH_COMPONENTS_H