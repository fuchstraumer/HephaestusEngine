#version 330 core
// These are our intake components, nothing really changes compared
// to normal shader code just that we specify a datatype and work magic
// later in this shader
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 lightPos;

out vec4 worldPosition;
out vec3 worldNormal;
out vec3 fragPos;
out vec2 frag_uv;

void main(){
	// This conversion is exactly how we do it in our other code as well
	vec4 decodedPosition = vec4(position,1.0f);
	//decodedPosition.xyz = decodedPosition.xyz * (1.0 / 256);
	frag_uv = uv;
	worldNormal = mat3(transpose(inverse(model))) * normal;

	// And now for our good ol' standard OpenGL transformations
	worldPosition = decodedPosition;
	fragPos = vec3(model * decodedPosition);
	gl_Position = projection * view * model * decodedPosition;
}