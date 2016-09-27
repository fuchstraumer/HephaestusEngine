#version 330 core

// This part of the shader requires much less work.
// simple passed input from vertex shader
in vec4 worldPosition;
in vec3 worldNormal;
in vec3 fragPos;

// color uniforms
uniform vec3 lightPos; 
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;


// color that gets written to the display
out vec4 outColor;

void main(){

	// Ambient lighting
	float ambientStrength = 0.2f;
	vec3 ambient = ambientStrength*lightColor;

	// Diffuse
	vec3 lightDir = normalize(lightPos - fragPos);
	vec3 norm = worldNormal;
	float diff = max(dot(norm,lightDir),0.0);
	vec3 diffuse = diff * lightColor;

	// Specular
	float specularStrength = 0.5f;
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir,reflectDir),0.0),32);
	vec3 specular = specularStrength * spec * lightColor;

	// Resultant lighting
	vec3 result = (ambient + diffuse + specular) * objectColor;
	outColor = vec4(result,1.0f);
}