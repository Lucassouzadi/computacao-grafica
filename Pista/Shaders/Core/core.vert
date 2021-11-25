#version 330 core

layout ( location = 0 ) in vec3 vPosition;
layout ( location = 1 ) in vec3 vNormal;
layout ( location = 2 ) in vec2 vTexture;

out vec3 fragPosition;
out vec3 interpolatedFragNormal;
out vec2 TexCoord;

uniform mat4 translate;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vec4 worldPosition = model * vec4(vPosition, 1.0);
	
	gl_Position = projection * view * worldPosition;
	fragPosition = vec3(worldPosition);
	interpolatedFragNormal = mat3(transpose(inverse(model))) * vNormal;
	TexCoord = vec2(vTexture.x, 1.0 - vTexture.y);
}
