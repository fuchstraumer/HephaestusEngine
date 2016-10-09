#version 330 core
// These are our intake components, nothing really changes compared
// to normal shader code just that we specify a datatype and work magic
// later in this shader
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 uv;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 lightPos;

out vec3 worldNormal;
out vec3 worldPosition;
out vec3 fragPos;
out vec3 frag_uv;
out vec2 texCoord;

void main(){
	worldPosition = position;
	vec4 Position = vec4(position,1.0f);
	worldNormal = mat3(transpose(inverse(model))) * normal;

	// Get tile coords
	frag_uv = uv;

	// And now for our good ol' standard OpenGL transformations
	fragPos = vec3(Position * model);
	gl_Position = projection * view * model * Position;
}