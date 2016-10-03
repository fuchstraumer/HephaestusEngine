#version 330 core

uniform sampler2D sampler;
uniform float timer;


varying vec2 frag_uv;

void main(){
	vec2 uv = vec2(timer,fragment_uv.t);
	gl_FragColor = texture2D(sampler,uv);
}