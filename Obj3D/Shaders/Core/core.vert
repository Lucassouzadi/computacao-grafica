#version 330 core

layout ( location = 0 ) in vec3 vPosition;
layout ( location = 1 ) in vec3 vNormal;
layout ( location = 2 ) in vec2 vTexture;

out vec3 color;

uniform mat4 translate;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	color = vec3(vTexture, 0.5);
	gl_Position = translate * vec4(vPosition, 1.0);
}