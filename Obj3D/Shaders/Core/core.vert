#version 330 core

layout ( location = 0 ) in vec3 vPosition;
layout ( location = 1 ) in vec3 vNormal;
layout ( location = 2 ) in vec2 vTexture;

out vec3 color;
out vec2 TexCoord;

uniform mat4 translate;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	color = (vNormal+1.0)*(vNormal+1.0)/4.0;
	gl_Position = projection * view * model * vec4(vPosition, 1.0);
	TexCoord = vec2(vTexture.x, vTexture.y);
}