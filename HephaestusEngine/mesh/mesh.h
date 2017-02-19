#pragma once
#ifndef MESH_H
#define MESH_H
#include "stdafx.h"
#include "MeshComponents.h"
#include "..\rendering\shader.h"

template<typename _vertex_type, typename _index_type = index_t>
class Mesh {
	typedef _vertex_type vertex_type;
	typedef _index_type index_type;
public:
	
	// Clears this mesh and attempts to free memory using "shrink_to_fit()"
	void Clear();

	// Returns number of indices in this mesh
	index_type GetNumIndices(void) const;
	// Returns number of vertices in this mesh
	index_type GetNumVerts(void) const;
	// Returns number of triangles in this mesh
	index_type GetNumTriangles(void) const;

	// Returns references to the relevant elements
	const vertex_type& GetVertex(index_type index) const;
	const index_type& GetIndex(index_type index) const;
	const triangle_t<index_type>& GetTriangle(index_type t_index) const;

	// Functions for adding items to the mesh. Most return indices to the relevant container

	// Add the vertex to the mesh and return it's index
	index_type AddVert(const vertex_type& vert);

	// Add triangle to the mesh, using three indices specified, return index
	index_type AddTriangle(const index_type &i0, const index_type &i1, const index_type &i2);

	// Adds exiting triangle "tri" to the mesh.
	index_type AddTriangle(const triangle_t<index_type> & tri);

	// Get the vertex in between the two vertices given by i0 and i1
	vertex_type GetMiddlePoint(const index_type & i0, const index_type & i1) const;

	// Get the vertex in between the input vertices v0 and v1
	vertex_type GetMiddlePoint(const vertex_type& v0, const vertex_type& v1) const;

	// Updates model matrix for this object.
	void UpdateModelMatrix();

	// Builds data needed to render this object with OpenGL
	void BuildRenderData(const ShaderProgram & shader);

	// Renders this object using the shader given.
	void Render(ShaderProgram & shader);
	
	// Members
	
	// Matrix defining this mesh's world transformation
	glm::mat4 Model;

	// This matrix is commonly used by the fragment shader. Cheaper to build it on the CPU once instead
	// of building it on the GPU for each vertex.
	glm::mat4 NormTransform;

	// This vector defines the position of this mesh in the world (barycentric)
	glm::vec3 Position;

	// Scale of this object
	glm::vec3 Scale;

	// Angle of this object relative to world axis
	glm::vec3 Angle = glm::vec3(0.0f, 0.0f, 0.0f);

private:
	// Private members.

	// Main data containers.
	std::vector<index_type> indices;
	std::vector<vertex_type> vertices;
	std::vector<triangle_t<index_type>> triangles;

	// Used for GL buffer/array objects
	GLuint VAO, VBO, EBO;

	// Builds a model matrix using given inputs. 
	glm::mat4 buildModelMatrix(glm::vec3 position, glm::vec3 scale, glm::vec3 angle);

};

template<typename vertex_type, typename index_type>
void Mesh<vertex_type, index_type>::Clear() {
	vertices.clear();
	indices.clear();
	triangles.clear();
	vertices.shrink_to_fit();
	indices.shrink_to_fit();
	triangles.shrink_to_fit();
}

// Return number of indices in this mesh
template<typename vertex_type, typename index_type>
index_type Mesh<vertex_type, index_type>::GetNumIndices() const {
	return static_cast<index_type>(indices.size());
}

// Return number of verts in this mesh
template<typename vertex_type, typename index_type>
index_type Mesh<vertex_type, index_type>::GetNumVerts() const {
	return static_cast<index_type>(vertices.size());
}

// Return number of triangles in this
template<typename vertex_type, typename index_type>
index_type Mesh<vertex_type, index_type>::GetNumTriangles() const {
	return static_cast<index_type>(triangles.size());
}

// Get vertex reference at index
template<typename vertex_type, typename index_type>
const vertex_type& Mesh<vertex_type, index_type>::GetVertex(index_type index) const {
	return vertices[index];
}

// Get index reference at index
template<typename vertex_type, typename index_type>
const index_type& Mesh<vertex_type, index_type>::GetIndex(index_type index) const {
	return indices[index];
}

// Get tri at index and return a reference
template<typename vertex_type, typename index_type>
const triangle_t<index_type>& Mesh<vertex_type, index_type>::GetTriangle(index_type t_index) const {
	return triangles[t_index];
}

// Add vert and return index to it
template<typename vertex_type, typename index_type>
index_type Mesh<vertex_type, index_type>::AddVert(const vertex_type& vert) {
	vertices.push_back(vert);
	if (vertices.size() > std::numeric_limits<index_type>::max()) {
		std::cerr << "Size of vertices container is now too large to be represented by selected index type. Use a larger index type." << std::endl;
		throw("MESH::ADD_VERT::L144: Insufficiently large index type for mesh if a vertice is added.");
	}
	return static_cast<index_t>(vertices.size() - 1);
}

// Add triangle using three indices and return a reference to this triangle
template<typename vertex_type, typename index_type>
index_type Mesh<vertex_type, index_type>::AddTriangle(const index_type &i0, const index_type &i1, const index_type &i2) {
	indices.push_back(i0);
	indices.push_back(i1);
	indices.push_back(i2);
	triangle_t<index_type> newTri(i0, i1, i2);
	triangles.push_back(std::move(newTri));
	index_t val = static_cast<index_t>(triangles.size() - 1);
	return val;
}

// Add a triangle using a reference to a triangle: adding the indices in the triangle, too
template<typename vertex_type, typename index_type>
index_type Mesh<vertex_type, index_type>::AddTriangle(const triangle_t<index_type> &tri) {
	indices.push_back(tri.i0);
	indices.push_back(tri.i1);
	indices.push_back(tri.i2);
	triangles.push_back(tri);
	return static_cast<index_t>(triangles.size() - 1);
}

// Gets the middle point in between two vertices, given by the indices into this objects vertex container
template<typename vertex_type, typename index_type>
vertex_type Mesh<vertex_type, index_type>::GetMiddlePoint(const index_type &i0, const index_type &i1) const {
	vertex_type res;
	auto&& v0 = GetVertex(i0);
	auto&& v1 = GetVertex(i1);
	res.Position = (v0.Position + v1.Position) / 2.0f;
	return res;
}

// Gets the middle point between two vertices, given by references to the verts in question
template<typename vertex_type, typename index_type>
vertex_type Mesh<vertex_type, index_type>::GetMiddlePoint(const vertex_type &v0, const vertex_type &v1) const {
	vertex_type res;
	res.Position = (v0.Position + v1.Position) / 2.0f;
	res.Normal = glm::normalize(res.Position - glm::vec3(0.0f));
	return res;
}

template<typename vertex_type, typename index_type>
glm::mat4 Mesh<vertex_type, index_type>::buildModelMatrix(glm::vec3 position, glm::vec3 scale, glm::vec3 angle) {
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), position);
	glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), angle.x, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), angle.y, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rotZ = glm::rotate(glm::mat4(1.0f), angle.z, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 result = scaleMatrix * rotX * rotY * rotZ * translationMatrix;
	return result;
};

// Update the model matrix, assuming we've changed one of the attributes of this mesh
template<typename vertex_type = vertex_t, typename index_type = index_t>
void Mesh<vertex_type, index_type>::UpdateModelMatrix() {
	Model = buildModelMatrix(Position, Scale, Angle);
	NormTransform = glm::transpose(glm::inverse(Model));
}

// Builds mesh data for this object
template<typename vertex_type, typename index_type>
void Mesh<vertex_type, index_type>::BuildRenderData(const ShaderProgram& shader) {

	// Generate the VAO and buffers for this mesh.
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO); glGenBuffers(1, &EBO);

	// Bind the VAO and begin setting things up.
	glBindVertexArray(VAO);

	// Bind the vertex buffer and fill it with data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, GetNumVerts() * sizeof(vertex_t), &(vertices[0]), GL_STATIC_DRAW);

	// Bind the element array (indice) buffer and fill it with data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, GetNumIndices() * sizeof(index_t), &(indices[0]), GL_STATIC_DRAW);

	// Pointer to the position attribute of a vertex
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Pointer to the normal of a vertex
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (GLvoid*)offsetof(vertex_type, Normal));
	glEnableVertexAttribArray(1);

	// Texture coordinate of a vertex
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (GLvoid*)offsetof(vertex_type, UV));
	glEnableVertexAttribArray(2);

	// Lighting attributes for a vertex
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (GLvoid*)offsetof(vertex_type, Lighting));
	glEnableVertexAttribArray(3);

	// Setup model matrix.
	Model = buildModelMatrix(Position, Scale, Angle);

	// Set transformation matrix for transforming normal to world-space,
	// used to pass a correct normal to the fragment shader. Cheaper to precalculate here
	// then calculate each frame on the GPU
	NormTransform = glm::transpose(glm::inverse(Model));

	// Unbind the VAO since we're done with it.
	glBindVertexArray(0);

}

template<typename vertex_type, typename index_type>
inline void Mesh<vertex_type, index_type>::Render(ShaderProgram& shader){

	// Activate shader passed in.
	shader.Use();

	// Bind the VAO we will be using.
	glBindVertexArray(VAO);

	// Get the location of the model from the shader's map object, and set the model matrix in the shader
	GLint modelLoc = shader.GetUniformLocation("model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Model));

	// Perform the same process for the normal transformation matrix.
	GLint normTLoc = shader.GetUniformLocation("normTransform");
	glUniformMatrix4fv(normTLoc, 1, GL_FALSE, glm::value_ptr(NormTransform));

	// Draw this object using triangles and the EBO constructed earlier (glDrawElements = indexed drawing)
	glDrawElements(GL_TRIANGLES, GetNumIndices(), GL_UNSIGNED_INT, 0);

	// Unbind the VAO since we're done drawing it.
	glBindVertexArray(0);
}


#endif // !MESH_H