#version 330 core
#define NUM_POINT 8
#define NUM_SPOT 8

in mat3 TBN;
in vec2 texCoords;
in vec3 fragmentPos;
in vec3 normVec;

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

//Shadows
uniform sampler2D shadow;
uniform mat4 lightSpaceMatrix;

uniform vec3 viewPos;

uniform bool useNormalTexture;

void calcDirectional();
void calcPointLight();
void calcSpotLight();

vec3 col3;
vec3 norm;
vec3 diffuseColour;
vec3 specularColour;
vec3 viewDir;

int i;

float calcShadow(vec3 lightDir){
	//Perform calculation here because when multiple lights have shadows cant use vert
	vec4 lPos = lightSpaceMatrix * vec4(fragmentPos, 1.0);
	//Perform perspective transform
	vec3 lPos3 = (lPos.xyz / lPos.w);
	//Change range to [0,1]
	lPos3 = lPos3 * 0.5 + 0.5;
	float closestDepth = texture(shadow, lPos3.xy).r;
	if(lPos3.z > 1.0){
        return 0.0;
	}
	//Compare depth (with small bias) to shadowmap
	float bias = max(0.05 * (1.0 - dot(norm, lightDir)), 0.005);
	return lPos3.z - bias > closestDepth ? 1.0 : 0.0;
}

void main(){
	//Calculate normal
	if(useNormalTexture) {
		//Move into correct range
		norm = normalize(texture(normalMap, texCoords).rgb * 2.0 - 1.0);
		//Transform to world space
		norm = normalize(TBN * norm);
	} else {
		norm = normVec;
	}
	//Calculate View Directional
	viewDir = normalize(viewPos - fragmentPos);
	//Calculate texture colours
	diffuseColour = texture(diffuse, texCoords).rgb;
	specularColour = texture(specular, texCoords).rgb * 0.3;
	//Add ambient light
	col3 = diffuseColour * ambient;
	//Add emission
	//col3 += texture(emissionMap, texCoords).rgb;
	//Add directional light
	if(numDirLights!=0){
		calcDirectional();
	}
	//Add point lights
	for(i=0;i<numPointLights;i++){
		calcPointLight();
	}
	//Add spotlights
	for(i=0;i<numSpotLights;i++){
		calcSpotLight();
	}
	color = vec4(col3, 1.0);
}

void calcDirectional(){
	vec3 lightDir = normalize(-dirLight.direction);
	//Blinn-Phong
	vec3 halfDir = normalize(lightDir + viewDir);
	float diff = max(dot(norm, lightDir), 0.0);
	float spec = max(0.0, pow(max(dot(norm, halfDir), 0.0), shininess));
	float shadow = calcShadow(lightDir);
	col3 += (diff * diffuseColour + spec * specularColour) * dirLight.colour * (1.0 - shadow);
}

void calcPointLight(){
	vec3 lightDir = normalize(pointLights[i].position - fragmentPos);
	//Blinn-Phong
	vec3 halfDir = normalize(lightDir + viewDir);
	float dist = length(pointLights[i].position - fragmentPos);
	float attenuation = 1.0 / (pointLights[i].constant + pointLights[i].linear * dist + 
    		    pointLights[i].quadratic * (dist * dist));  
	float diff = max(dot(norm, lightDir), 0.0);
	float spec = max(0.0, pow(max(dot(norm, halfDir), 0.0), shininess));
	col3 += (diff * diffuseColour + spec * specularColour) * pointLights[i].colour * attenuation;
}

void calcSpotLight(){
	vec3 lightDir = normalize(spotLights[i].position - fragmentPos);
	//Blinn-Phong
	vec3 halfDir = normalize(lightDir + viewDir);
	float dist = length(spotLights[i].position - fragmentPos);
	float attenuation = 1.0 / (spotLights[i].constant + spotLights[i].linear * dist + 
    		    spotLights[i].quadratic * (dist * dist));
	float theta = dot(lightDir, normalize(-spotLights[i].direction));
	float epsilon = spotLights[i].cutOff - spotLights[i].outerCutOff;
	float intensity = clamp((theta - spotLights[i].outerCutOff) / epsilon, 0.0, 1.0);
	float diff = max(dot(norm, lightDir), 0.0);
	float spec = max(0.0, pow(max(dot(norm, halfDir), 0.0), shininess));
	col3 += (diff * diffuseColour + spec * specularColour) * spotLights[i].colour * attenuation * intensity;
}
