#version 330 core

in vec3 color;
in vec2 TexCoord;
out vec4 frag_color;

uniform sampler2D texture1;
uniform float alpha;
uniform bool hasTexture;

void main(){
	if (hasTexture) {
		frag_color = texture(texture1, TexCoord);
	} else {
		frag_color = vec4(color, alpha);
	}
}