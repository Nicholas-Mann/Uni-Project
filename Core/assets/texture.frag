#version 430

in vec2 v2fTexCoord;
in vec3 v2fNormal;
in vec3 FragPos;

layout( location = 0 ) out vec3 oColor;

struct Light{
	vec3 position;
	vec3 color;
	float intensity;
};

layout( location = 3 ) uniform vec3 uViewPos;
layout( location = 4 ) uniform vec3 uLightDir;
layout( location = 5 ) uniform vec3 uLightDiffuse;
layout( location = 6 ) uniform vec3 uSceneAmbient;
layout( location = 7 ) uniform float uMaterialAmbient;
layout( location = 8 ) uniform float uMaterialDiffuse;
layout( location = 9 ) uniform float uMaterialSpec;
layout( location = 10 ) uniform vec3 uMaterialEmissive;
layout( location = 11 ) uniform float uMaterialShininess;
layout( location = 12 ) uniform Light uLights[3];
layout( binding = 0 ) uniform sampler2D uTexture;

void main()
{
	vec3 ambient = vec3(0.0);
	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);

	vec3 norm = normalize(v2fNormal);
	vec3 viewDir = normalize(uViewPos - FragPos);

	for (int i = 0; i < 3; ++i) {
		// Compute distance and attenuation
		vec3 lightDir = uLights[i].position - FragPos;
		float distance = length(lightDir);
		lightDir = normalize(lightDir);
		float attenuation = uLights[i].intensity / (distance * distance);

		// Ambient
		ambient += uMaterialAmbient * uLights[i].color * attenuation;

		// Diffuse
		float diff = max(dot(norm, lightDir), 0.0);
		diffuse += diff * uMaterialDiffuse * uLights[i].color * attenuation;

		// Specular
		vec3 halfwayDir = normalize(lightDir + viewDir);
		float spec = pow(max(dot(norm, halfwayDir), 0.0), uMaterialShininess);
		specular += spec * uMaterialSpec * uLights[i].color * attenuation;
	}

	float nDotL = max( 0.0, dot( v2fNormal, uLightDir ) );

	vec3 globalLight = uSceneAmbient + nDotL * uLightDiffuse;

	// Load the texture color
	vec3 texColour = texture( uTexture, v2fTexCoord ).rgb;

	vec3 result = (ambient + diffuse + specular + globalLight) * texColour + uMaterialEmissive;

	oColor = result;
}
