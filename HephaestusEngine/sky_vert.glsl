#version 330 core

uniform mat4 matrix;

attribute vec4 pos;
attribute vec3 norm;
attribute vec2 uv;

varying vec2 frag_uv; // Sky needs to vary texture UV to create day/night cycle


void main(){
	gl_Position = matrix * position;
	fragment_uv = uv;
}