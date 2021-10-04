#version 330 core

in vec3 color;
in vec2 TexCoord;
out vec4 frag_color;

uniform sampler2D texture1;
uniform float alpha;

void main(){
	frag_color = texture(texture1, TexCoord) * vec4(color, alpha);
}