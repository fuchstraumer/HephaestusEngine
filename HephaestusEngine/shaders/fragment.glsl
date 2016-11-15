#version 330 core

// This part of the shader requires much less work.
// simple passed input from vertex shader
in vec3 worldNormal;
in vec3 fragPos;
in vec3 fragUV;

// color uniforms
uniform vec3 lightPos; 
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform sampler2DArray texSampler;

// color that gets written to the display
out vec4 outColor;

void main(){
	vec4 color = texture(texSampler, fragUV);

	// Ambient lighting
	float ambientStrength = 0.3f;
	vec3 ambient = ambientStrength*lightColor;

	// Diffuse
	float diffuseStrength = 0.75f;
	vec3 lightDir = normalize(lightPos - fragPos);
	vec3 norm = worldNormal;
	float diff = max(dot(norm,lightDir),0.0);
	vec3 diffuse = diffuseStrength * diff * lightColor;

	// Specular
	float specularStrength = 0.15f;
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir,reflectDir),0.0),32);
	vec3 specular = specularStrength * spec * lightColor;

	// Resultant lighting
	vec4 lightResult = vec4(ambient+diffuse+specular,1.0f);
	if(color.a < 0.1)
		discard;
	outColor = lightResult * color;
}