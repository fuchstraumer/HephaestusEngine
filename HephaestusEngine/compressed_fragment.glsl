#version 330 core

// This part of the shader requires much less work.
// simple passed input from vertex shader
in vec3 worldPosition;
in vec3 worldNormal;
in vec3 frag_uv;
in vec2 texCoord;
in vec3 fragPos;

// color uniforms
uniform vec3 lightPos; 
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform sampler2DArray texSampler;
uniform float tileSize;
uniform float tileCount;

// color that gets written to the display
out vec4 outColor;

void main(){
	// Get texture coordinates and use fractional component to get correct wrapping
	vec4 color = texture(texSampler, frag_uv);
	

	// Ambient lighting
	float ambientStrength = 0.4f;
	vec3 ambient = ambientStrength*lightColor;

	// Diffuse
	float diffuseStrength = 0.7f;
	vec3 lightDir = normalize(lightPos - fragPos);
	vec3 norm = worldNormal;
	float diff = max(dot(norm,lightDir),0.0);
	vec3 diffuse = diffuseStrength * diff * lightColor;

	// Specular
	float specularStrength = 0.2f;
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir,reflectDir),0.0),32);
	vec3 specular = specularStrength * spec * lightColor;

	// Resultant lighting
	vec4 lightResult = vec4(ambient+diffuse+specular,1.0f);
	outColor = lightResult * color;
}