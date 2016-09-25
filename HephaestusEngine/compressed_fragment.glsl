#version 330 core

// This part of the shader requires much less work.
// simple passed input from vertex shader
in vec4 worldPosition;
in vec4 worldNormal;
// color that gets written to the display
out vec4 outColor;

void main(){
	// Below is a commented-out line for calculating vertex normals 
	// in the fragment shader. Useful if we just want to recalculate, 
	// or cut our vert size down even more!
	//vec3 normal = normalize(cross(dFdy(worldPosition.xyz), dFdx(worldPosition.xyz)));

	// We're just using a flat shader for this demo
	outColor = vec4(abs(worldNormal.xyz) * 0.5 + vec3(0.5,0.5,0.5),1.0);
}