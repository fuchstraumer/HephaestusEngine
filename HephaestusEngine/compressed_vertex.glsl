#version 330 core
// These are our intake components, nothing really changes compared
// to normal shader code just that we specify a datatype and work magic
// later in this shader
in uvec4 position;
in uint normal; 

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

out vec4 worldPosition;
out vec4 worldNormal;

// returns +/- 1, useful in a bit
vec2 signnotzero(vec2 v){
	return vec2((v.x > = 0.0) ? +1.0 : -1.0, (v.y >= 0.0) ? +1.0 : -1.0);
}

void main{
	// This conversion is exactly how we do it in our other code as well
	vec4 decodedPosition = position;
	decodedPosition.xyz = decodedPosition.xyz * (1.0 / 256);

	// This one is a bit fancier
	uint encodedX = (normal >> 8u) & 0xFFu;
	uint encodedY = (normal) & 0xFFu;

	// Map to range -1 <> 1 (normalize, sorta)
	vec2 e = vec2(encodedX, encodedY);
	e = e*vec2(1.0 / 127.5, 1.0 / 127.5);
	e = e - vec2(1.0,1.0);
	// Now decode the normal using listing from here (also remember
	// that polyvox did this first!) http://jcgt.org/published/0003/02/01/
	vec3 v = vec3(e.xy, 1.0 - abs(e.x) - abs(e.y));
	if (v.z < 0) v.xy = (1.0 - abs(v.yx)) * signnotzero(v.xy);	
	worldNormal.xyz = normalize(v);
	worldNormal.w = 1.0;

	// And now for our good ol' standard OpenGL transformations
	worldPosition = modelMatrix * decodedPosition;
	vec4 cameraPosition = viewMatrix * worldPosition;
	gl_Position = projectionMatrix * cameraPosition;
}