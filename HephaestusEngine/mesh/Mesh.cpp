#include "stdafx.h"
#include "Mesh.h"
#include "glm/gtx/vector_angle.hpp"

__inline float maptosphere(float const &a, float const &b, float const &c) {
	float result;
	result = a * sqrt(1 - (b*b / 2.0f) - (c*c / 2.0f) + ((b*b)*(c*c) / 3.0f));
	return result;
}

void Mesh::Clear() {
	Vertices.clear(); Indices.clear();
	Triangles.clear(); Faces.clear();
	Edges.clear();
	Vertices.shrink_to_fit();
	Indices.shrink_to_fit();
	Triangles.shrink_to_fit();
	Faces.shrink_to_fit();
}

// Return number of indices in this mesh
GLsizei Mesh::GetNumIndices() const {
	return static_cast<GLsizei>(Indices.size());
}

// Return number of verts in this mesh
GLsizei Mesh::GetNumVerts() const {
	return static_cast<GLsizei>(Vertices.size());
}

// Get vertex reference at index
const vertex_t & Mesh::GetVertex(index_t index) const {
	return Vertices[index];
}

// Get index reference at index
const index_t & Mesh::GetIndex(index_t index) const {
	return Indices[index];
}

// Get face at index and return a reference
const face_t& Mesh::GetFace(index_t f_index) const {
	return Faces[f_index];
}

// Get tri at index and return a reference
const triangle_t & Mesh::GetTri(index_t t_index) const {
	return Triangles[t_index];
}
// Add vert and return index to it
index_t Mesh::AddVert(const vertex_t & vert) {
	Vertices.push_back(vert);
	return static_cast<index_t>(Vertices.size() - 1);
}

// Add triangle using three indices and return a reference to this triangle
index_t Mesh::AddTriangle(const index_t &i0, const index_t &i1, const index_t &i2){
	Indices.push_back(i0);
	Indices.push_back(i1);
	Indices.push_back(i2);
	triangle_t newTri(i0, i1, i2);
	Triangles.push_back(newTri);
	index_t val = (index_t)Triangles.size() - 1;
	// The triangle actually stores the edges, as well.
	// The value the key points to tells us which triangle
	// this edge is used by
	// This is a multimap, so multiple copies of the same 
	// key value can exist but will have different values.
	// This lets us find all triangles that use an edge.
	Edges.insert(std::pair<edge_key, index_t>(newTri.e0, val));
	Edges.insert(std::pair<edge_key, index_t>(newTri.e1, val));
	Edges.insert(std::pair<edge_key, index_t>(newTri.e2, val));
	return val;
}

// Add face and return index to it
index_t Mesh::AddFace(const face_t& face) {
	Faces.push_back(face);
	return (index_t)Faces.size() - 1;
}

face_t Mesh::CreateFace(const index_t &i0, const index_t &i1, const index_t &i2, const index_t &i3) {
	index_t t0 = AddTriangle(i0, i1, i3);
	index_t t1 = AddTriangle(i1, i2, i3);
	return face_t(t0, t1);
}

face_t Mesh::CreateFace(const index_t& t0, const index_t& t1) const {
	return face_t(t0, t1);
}

void Mesh::BuildRenderData(ShaderProgram& shader){
	shader.Use();
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO); glGenBuffers(1, &EBO);
	glBindVertexArray(this->VAO);
	// Bind the vertex buffer and then specify what data it will be loaded with
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, GetNumVerts() * sizeof(vertex_t), &(Vertices[0]), GL_DYNAMIC_DRAW);
	// Bind the element array (indice) buffer and fill it with data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, GetNumIndices() * sizeof(index_t), &(Indices.front()), GL_DYNAMIC_DRAW);
	// Pointer to the position attribute of a vertex
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (GLvoid*)0);
	// Pointer to the normal attribute of a vertex
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (GLvoid*)offsetof(vertex_t, Normal));
	// Pointer to the UV attribute of a vertex
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (GLvoid*)offsetof(vertex_t, UV));
	// Pointer to AO attribute of a vertex
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (GLvoid*)offsetof(vertex_t, ao));
	// Set model
	Model = glm::translate(Model, Position);
	NormTransform = glm::transpose(glm::inverse(Model));
	modelLoc = shader.GetUniformLocation("model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Model));
	normTLoc = shader.GetUniformLocation("normTransform");
	glUniformMatrix4fv(normTLoc, 1, GL_FALSE, glm::value_ptr(NormTransform));
	glBindVertexArray(0);
}

void Mesh::Render(ShaderProgram & shader){
	shader.Use();
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, GetNumIndices(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
