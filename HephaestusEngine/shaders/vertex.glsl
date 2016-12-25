#version 330 core
// These are our intake components, nothing really changes compared
// to normal shader code just that we specify a datatype and work magic
// later in this shader
layout(location = 0) in vec3 position;
layout(location = 1) in uint normal;
layout(location = 2) in vec3 uv;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 lightPos;

out uint vNormal;
out vec3 vPos;
out vec3 vUV;

void main(){

	vec4 Position = vec4(position,1.0f);
	vNormal = normal;

	// Get tile coords
	vUV = uv;

	// And now for our good ol' standard OpenGL transformations
	vPos = vec3(Position * model);
	gl_Position = projection * view * model * Position;
}