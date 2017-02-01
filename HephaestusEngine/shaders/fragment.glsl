#version 330 core

// This part of the shader requires much less work.
// simple passed input from vertex shader
in vec3 vNormal;
in vec3 vPos;
in vec3 vUV;
in float vAO;

// color uniforms
uniform vec3 lightPos; 
uniform vec3 viewPos;
uniform mat4 normTransform;
uniform sampler2DArray textureAtlas;

// color that gets written to the display
out vec4 outColor;

vec2 getSign(vec2 v){
	return vec2((v.x >= 0.0) ? +1.0 : -1.0, (v.y >= 0.0) ? +1.0 : -1.0);
}

void main(){
	vec4 color = texture(textureAtlas, vUV);
	vec3 lightColor = vec3(0.99f, 0.99f, 0.88f);
	// Ambient lighting
	float ambientStrength = 0.3f;
	vec3 ambient = ambientStrength*lightColor;

	// Diffuse
	float diffuseStrength = 0.75f;
	vec3 lightDir = normalize(lightPos - vPos);
	vec3 norm = vNormal;
	float diff = max(dot(norm,lightDir),0.0);
	vec3 diffuse = diffuseStrength * diff * lightColor;

	// Specular
	float specularStrength = 0.15f;
	vec3 viewDir = normalize(viewPos - vPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir,reflectDir),0.0),32);
	vec3 specular = specularStrength * spec * lightColor;

	// Ambient occlusion
	vec3 aoColor = vec3(0.05f, 0.05f, 0.05f);
	vec3 aoResult = (vAO * -0.08f) * aoColor;

	// Resultant lighting
	vec4 lightResult = vec4(ambient+diffuse+specular+aoResult,1.0f);
	if(color.a < 0.1)
		discard;
	outColor = lightResult * color;
}