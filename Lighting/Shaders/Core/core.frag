#version 330 core

in vec3 interpolatedFragNormal;
in vec3 fragPosition;
in vec2 TexCoord;
out vec4 frag_color;

uniform vec3 objColor;

uniform vec3 aColor;			// Ia
uniform float aStrength;		// ka

uniform vec3 lightColor;		// Ip
uniform vec3 lightPosition;

uniform vec3 eyePosition;

uniform sampler2D texture1;
uniform float alpha;
uniform bool hasTexture;

void main(){

	vec3 ambient = aStrength * aColor;										// I = Ia * ka

	vec3 fragNormal = normalize(interpolatedFragNormal);					// N
	vec3 lightDir = lightPosition - fragPosition;							// VL
	vec3 lightDirNorm = normalize(lightDir);								// L
	float cossin = dot(fragNormal, lightDirNorm);							// cos 0 = N.L
	float reflectionCoefficient = 0.5;										// kd
	float attenuationFactor = 800.0 / pow(length(lightDir), 2);				// fatt = 1 / d²
	vec3 diffuse = attenuationFactor * lightColor * max(cossin, 0.0) * reflectionCoefficient;	// Id = fatt * Ip * N.L * kd
	
	float specularStrength = 0.5;											// ks
	float q = 50;															// q shininess
	vec3 viewDir = normalize(eyePosition - fragPosition);					// V = |P->V|
	float Ad = dot(fragNormal, lightDirNorm);								// Ad = N.L
	//vec3 reflectDir = fragNormal * (1.0 - Ad) - lightDirNorm;				// R = N * (1-Ad) - L
	vec3 reflectDir = reflect(-lightDirNorm, fragNormal);					// R = reflect(L,N);
	float As = max(dot(viewDir, reflectDir), 0.0);							// As = V.R
	float s = pow(As, q);													// s = As^q
	vec3 specular = specularStrength * s * lightColor;						// Is = ks * s * Ip

	vec3 phong = vec3(0.0);
	phong += ambient;
	phong += diffuse;
	phong += specular;

	if (hasTexture)
		phong *= vec3(texture(texture1, TexCoord));
	else
		phong *= objColor;

	frag_color = vec4(phong, 1.0);
	//frag_color = vec4((fragNormal+1.0) / 2.0, 1.0);
	//frag_color = vec4(fragPosition/10.0, 1.0);
}
