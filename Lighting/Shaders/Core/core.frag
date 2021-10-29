#version 330 core

in vec3 interpolatedFragNormal ;
in vec3 fragPosition;
in vec2 TexCoord;
out vec4 frag_color;

uniform vec3 objColor;
uniform vec3 aColor;
uniform float aStrength;
uniform vec3 dColor;
uniform vec3 dPosition;
uniform sampler2D texture1;
uniform float alpha;
uniform bool hasTexture;

void main(){

	vec3 ambient = aStrength * aColor;

	vec3 fragNormal = normalize(interpolatedFragNormal);
	vec3 lightDir = dPosition - fragPosition;  
	float cossin = dot(fragNormal, normalize(lightDir));
	vec3 diffuse = max(cossin, 0.0) * dColor;
	
	vec3 specular = vec3(0.0);

	vec3 phong = (ambient + diffuse + specular);
	if (hasTexture)
		phong *= texture(texture1, TexCoord);
	else
		phong *= objColor;

	frag_color = vec4(phong, 1.0);
	//frag_color = vec4((fragNormal+1.0) / 2.0, 1.0);
	//frag_color = vec4(fragPosition/10.0, 1.0);
}