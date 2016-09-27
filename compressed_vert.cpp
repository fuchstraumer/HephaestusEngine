if (frontFace == true) { 
		index_t i0, i1, i2, i3; vertType vert0, vert1, vert2, vert3; // Using Points 0, 1, 2, 3 and Normal 0
		vert0.position = mesh.encodePosition(vertices[0]); vert1.position = mesh.encodePosition(vertices[1]); 
		vert2.position = mesh.encodePosition(vertices[2]); vert3.position = mesh.encodePosition(vertices[3]);
		vert0.normal = mesh.encodeNormal(normals[0]);  vert1.normal = mesh.encodeNormal(normals[0]); 
		vert2.normal = mesh.encodeNormal(normals[0]);  vert3.normal = mesh.encodeNormal(normals[0]);
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1); 
		i2 = this->mesh.addVert(vert2); i3 = this->mesh.addVert(vert3);
		this->mesh.addTriangle(i0, i1, i2); this->mesh.addTriangle(i0, i2, i3);
	}
	if (rightFace == true) {
		index_t i0, i1, i2, i3; vertType vert0, vert1, vert2, vert3; // Using Points 1, 4, 7, 2 and Normal 1
		vert0.position = mesh.encodePosition(vertices[1]); vert1.position = mesh.encodePosition(vertices[4]);
		vert2.position = mesh.encodePosition(vertices[7]); vert3.position = mesh.encodePosition(vertices[2]);
		vert0.normal = mesh.encodeNormal(normals[1]);  vert1.normal = mesh.encodeNormal(normals[1]);
		vert2.normal = mesh.encodeNormal(normals[1]);  vert3.normal = mesh.encodeNormal(normals[1]);
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1);
		i2 = this->mesh.addVert(vert2); i3 = this->mesh.addVert(vert3);
		this->mesh.addTriangle(i0, i1, i2); this->mesh.addTriangle(i0, i2, i3);
	}
	if (topFace == true) {
		index_t i0, i1, i2, i3; vertType vert0, vert1, vert2, vert3; // Using Points 3, 2, 7, 6 and Normal 2
		vert0.position = mesh.encodePosition(vertices[3]); vert1.position = mesh.encodePosition(vertices[2]);
		vert2.position = mesh.encodePosition(vertices[7]); vert3.position = mesh.encodePosition(vertices[6]);
		vert0.normal = mesh.encodeNormal(normals[2]);  vert1.normal = mesh.encodeNormal(normals[2]);
		vert2.normal = mesh.encodeNormal(normals[2]);  vert3.normal = mesh.encodeNormal(normals[2]);
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1);
		i2 = this->mesh.addVert(vert2); i3 = this->mesh.addVert(vert3);
		this->mesh.addTriangle(i0, i1, i2); this->mesh.addTriangle(i0, i2, i3);
	}
	if (leftFace == true) {
		index_t i0, i1, i2, i3; vertType vert0, vert1, vert2, vert3; // Using Points 5, 0, 3, 6 and Normal 3
		vert0.position = mesh.encodePosition(vertices[5]); vert1.position = mesh.encodePosition(vertices[0]);
		vert2.position = mesh.encodePosition(vertices[3]); vert3.position = mesh.encodePosition(vertices[6]);
		vert0.normal = mesh.encodeNormal(normals[3]);  vert1.normal = mesh.encodeNormal(normals[3]);
		vert2.normal = mesh.encodeNormal(normals[3]);  vert3.normal = mesh.encodeNormal(normals[3]);
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1);
		i2 = this->mesh.addVert(vert2); i3 = this->mesh.addVert(vert3);
		this->mesh.addTriangle(i0, i1, i2); this->mesh.addTriangle(i0, i2, i3);
	}
	if (bottomFace == true) {
		index_t i0, i1, i2, i3; vertType vert0, vert1, vert2, vert3; // Using Points 5, 4, 1, 0 and Normal 4
		vert0.position = mesh.encodePosition(vertices[5]); vert1.position = mesh.encodePosition(vertices[4]);
		vert2.position = mesh.encodePosition(vertices[1]); vert3.position = mesh.encodePosition(vertices[0]);
		vert0.normal = mesh.encodeNormal(normals[4]);  vert1.normal = mesh.encodeNormal(normals[4]);
		vert2.normal = mesh.encodeNormal(normals[4]);  vert3.normal = mesh.encodeNormal(normals[4]);
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1);
		i2 = this->mesh.addVert(vert2); i3 = this->mesh.addVert(vert3);
		this->mesh.addTriangle(i0, i1, i2); this->mesh.addTriangle(i0, i2, i3);
	}
	if (backFace == true) {
		index_t i0, i1, i2, i3; vertType vert0, vert1, vert2, vert3; // Using Points 4, 5, 6, 7 and Normal 5
		vert0.position = mesh.encodePosition(vertices[4]); vert1.position = mesh.encodePosition(vertices[5]);
		vert2.position = mesh.encodePosition(vertices[6]); vert3.position = mesh.encodePosition(vertices[7]);
		vert0.normal = mesh.encodeNormal(normals[5]);  vert1.normal = mesh.encodeNormal(normals[5]);
		vert2.normal = mesh.encodeNormal(normals[5]);  vert3.normal = mesh.encodeNormal(normals[5]);
		i0 = this->mesh.addVert(vert0); i1 = this->mesh.addVert(vert1);
		i2 = this->mesh.addVert(vert2); i3 = this->mesh.addVert(vert3);
		this->mesh.addTriangle(i0, i1, i2); this->mesh.addTriangle(i0, i2, i3);
	}