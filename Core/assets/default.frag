#version 430

in vec3 v2fColor;
in vec3 v2fNormal;
in vec3 FragPos;

layout( location = 0 ) out vec3 oColor;

struct Light{
	vec3 position;
	vec3 color;
	float intensity;
};

uniform vec3 uViewPos;
uniform vec3 uLightDir;
uniform vec3 uLightDiffuse;
uniform vec3 uSceneAmbient;
uniform float uMaterialAmbient;
uniform float uMaterialDiffuse;
uniform float uMaterialSpec;
uniform vec3 uMaterialEmissive;
uniform float uMaterialShininess;
uniform Light uLights[3];

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

    vec3 result = (ambient + diffuse + specular + globalLight) * v2fColor + uMaterialEmissive;

	oColor = result;
}
