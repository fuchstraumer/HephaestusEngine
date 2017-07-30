#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
layout (binding = 0) uniform sampler2DArray textureSampler;

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec3 vUV;

layout(push_constant) uniform _fragment_push {
	layout(offset = 192) vec4 lightColor;
	layout(offset = 208) vec4 lightPos;
	layout(offset = 224) vec4 viewPos;
} ubo;

layout(location = 0) out vec4 fragColor;

void main() {

	vec4 texcolor = texture(textureSampler, vUV);
	//vec4 color = vec4(0.0f, 0.9f, 0.1f, 1.0f);
	float ambient_strength = 0.2f;
	vec3 ambient = ambient_strength * ubo.lightColor.xyz;

	float diffuse_strength = 0.6f;
	vec3 light_dir = normalize(ubo.lightPos.xyz - vPosition);
	float diff = max(dot(vNormal, light_dir), 0.0f);
	vec3 diffuse = diffuse_strength * diff * ubo.lightColor.xyz;

	float specular_strength = 0.3f;
	vec3 view_dir = normalize(ubo.viewPos.xyz - vPosition);
	vec3 reflect_dir = reflect(-light_dir, vNormal);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0f), 32.0f);
	vec3 specular = specular_strength * spec * ubo.lightColor.xyz;

	vec4 light_result = vec4(ambient + diffuse + specular, 1.0f);
	fragColor = light_result * texcolor;
}