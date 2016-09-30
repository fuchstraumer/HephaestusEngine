#version 330 core

// This part of the shader requires much less work.
// simple passed input from vertex shader
in vec4 worldPosition;
in vec3 worldNormal;
in vec3 fragPos;

// Material struct
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
  
// color uniforms
uniform vec3 lightPos; 
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform Material material;

// color that gets written to the display
out vec4 outColor;

void main(){

	// Ambient lighting
	float ambientStrength = 0.3f;
	vec3 ambient = ambientStrength*lightColor;

	// Diffuse
	float diffuseStrength = 0.5f;
	vec3 lightDir = normalize(lightPos - fragPos);
	vec3 norm = worldNormal;
	float diff = max(dot(norm,lightDir),0.0);
	vec3 diffuse = diffuseStrength * diff * lightColor;

	// Specular
	float specularStrength = 0.3f;
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir,reflectDir),0.0),32);
	vec3 specular = specularStrength * spec * lightColor;

	// Resultant lighting
	vec3 result = (ambient + diffuse + specular) * objectColor;
	outColor = vec4(result,1.0f);
}