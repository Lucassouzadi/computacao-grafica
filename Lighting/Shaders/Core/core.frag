#version 330 core

in vec3 interpolatedFragNormal;
in vec3 fragPosition;
in vec2 TexCoord;
out vec4 frag_color;

uniform vec3 objColor;

uniform vec3 aColor;			// Ia

uniform vec3 lightColor;		// Ip
uniform vec3 lightPosition;

uniform vec3 eyePosition;

uniform sampler2D texture1;
uniform float alpha;
uniform bool hasTexture;

uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;

uniform float shininess;

void main(){

	vec3 ambient = aColor * ka;												// I = Ia * ka

	vec3 fragNormal = normalize(interpolatedFragNormal);					// N
	vec3 lightDir = lightPosition - fragPosition;							// VL
	vec3 lightDirNorm = normalize(lightDir);								// L
	float cossin = dot(fragNormal, lightDirNorm);							// cos 0 = N.L
	vec3 reflectionCoefficient = kd;										// kd
	float attenuationFactor = 600000.0 / pow(length(lightDir), 2);			// fatt = 1 / d�
	attenuationFactor = min(1.0f, attenuationFactor);						// fatt = 1 / d�
	vec3 diffuse = attenuationFactor * lightColor * max(cossin, 0.0) * reflectionCoefficient;	// Id = fatt * Ip * N.L * kd

	vec3 specularStrength = ks;												// ks
	vec3 viewDir = normalize(eyePosition - fragPosition);					// V = |P->V|
	float Ad = dot(fragNormal, lightDirNorm);								// Ad = N.L
	//vec3 reflectDir = fragNormal * (1.0 - Ad) - lightDirNorm;				// R = N * (1-Ad) - L
	vec3 reflectDir = reflect(-lightDirNorm, fragNormal);					// R = reflect(L,N);
	float As = max(dot(viewDir, reflectDir), 0.0);							// As = V.R
	float s = pow(As, shininess);											// s = As^shininess
	vec3 specular = attenuationFactor * specularStrength * s * lightColor;						// Is = ks * s * Ip

	vec3 phong = vec3(0.0);
	phong += ambient;
	phong += diffuse;
	phong += specular;

	if (hasTexture)
		phong *= vec3(texture(texture1, TexCoord));
	else
		phong *= objColor;

	frag_color = vec4(phong, 1.0);

	vec4 cor_fog = vec4(0.2f, 0.3f, 0.3f, 1.0f);

	float d = length(fragPosition - eyePosition);
	float fatm = min(max(100.0 / d - 0.2f, 0.0f), 1.0f); 
	frag_color = frag_color * fatm + (1.0f - fatm) * cor_fog;

	//frag_color = vec4((fragNormal+1.0) / 2.0, 1.0);
	//frag_color = vec4(fragPosition/10.0, 1.0);
}
