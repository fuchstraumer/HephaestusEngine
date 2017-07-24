#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 uv;

layout(push_constant) uniform _ubo {
	mat4 model;
	mat4 view;
	mat4 projection;
} ubo;

out gl_PerVertex {
	vec4 gl_Position;
};

layout(location = 0) out vec3 vPosition;
layout(location = 1) out vec3 vNormal;
layout(location = 2) out vec3 vUV;

void main() {
	vUV = uv;
	mat4 norm_transform = transpose(inverse(ubo.model));
	vNormal = normal; // * mat3(norm_transform);
	vPosition = vec3(vec4(position, 1.0f) * ubo.model);
	gl_Position = ubo.projection * ubo.view * ubo.model * vec4(position, 1.0f);
}