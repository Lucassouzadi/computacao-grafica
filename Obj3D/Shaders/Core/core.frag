#version 330 core

in vec3 color;
out vec4 frag_color;

uniform sampler2D texture1;
uniform float alpha;

void main(){
	frag_color = vec4(color, alpha);
}