#version 330 core
#define NUM_POINT 8
#define NUM_SPOT 8

in mat3 TBN;
in vec2 texCoords;
in vec3 fragmentPos;

out vec4 color;

struct DirectionalLight {
	vec3 direction;
	vec3 colour;
};
struct PointLight {
	vec3 position;
	vec3 colour;
	//Attenuation
	float constant;
	float linear;
	float quadratic;
};
struct SpotLight {
	vec3 position;
	vec3 direction;
	vec3 colour;
	float cutOff;
	float outerCutOff;
	//Attenuation
	float constant;
	float linear;
	float quadratic;
};
//Light properties
uniform DirectionalLight dirLight;
uniform PointLight[NUM_POINT] pointLights;
uniform SpotLight[NUM_SPOT] spotLights;
uniform int numDirLights;
uniform int numPointLights;
uniform int numSpotLights;
uniform vec3 ambient;
uniform float shininess;

//Textures
uniform sampler2D diffuse;
uniform sampler2D specular;
uniform sampler2D normalMap;
uniform sampler2D emissionMap;

uniform vec3 viewPos;

void calcDirectional();
void calcPointLight();
void calcSpotLight();

vec3 col3;
vec3 norm;
vec3 diffuseColour;
vec3 specularColour;
vec3 viewDir;

void main(){
	//Calculate normal
	//TODO: No normal map fix
	//Move into correct range
	norm = normalize(texture(normalMap, texCoords).rgb * 2.0 - 1.0);
	//	norm = vec3(0.0, 0.0, 1.0);
	//Transform to world space
	norm = normalize(TBN * norm);
	//Calculate View Directional
	viewDir = normalize(viewPos - fragmentPos);
	//Calculate texture colours
	diffuseColour = texture(diffuse, texCoords).rgb;
	specularColour = texture(specular, texCoords).rgb * 0.3;
	//Add ambient light
	col3 = diffuseColour * ambient;
	//Add emission
	col3 += texture(emissionMap, texCoords).rgb;
	//Add directional light
	calcDirectional();
	//Add point lights
	
	//Add spotlights
	
	//col3= texture(specular, texCoords).rgb;
	color = vec4(col3, 1.0);
}

void calcDirectional(){
	vec3 lightDir = normalize(-dirLight.direction);
	//Blinn-Phong
	vec3 halfDir = normalize(lightDir + viewDir);
	float diff = max(dot(norm, lightDir), 0.0);
	float spec = pow(max(dot(norm, halfDir), 0.0), shininess);
	col3 += (diff * diffuseColour + spec * specularColour) * dirLight.colour;
}

void calcPointLight(){

}

void calcSpotLight(){

}
