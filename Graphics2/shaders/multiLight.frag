#version 330 core
#define NUM_POINT 8
#define NUM_SPOT 8
#define TOTAL_LIGHTS 17
out vec4 color;

in vec3 tangentPosition;
in vec2 texCoords;
in vec3 tangentView;
in mat3 TBN;

struct DirectionalLight {
	vec3 direction;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 position;
	vec3 diffuse;
	vec3 specular;
	//Attenuation
	float constant;
	float linear;
	float quadratic;
};

struct SpotLight {
	vec3 position;
	vec3 direction;
	vec3 diffuse;
	vec3 specular;
	float cutOff;
	float outerCutOff;
	//Attenuation
	float constant;
	float linear;
	float quadratic;
};

//Material properties
uniform sampler2D diffuse;
uniform sampler2D specular;
uniform float shininess;
uniform sampler2D normalMap;
uniform sampler2D emissionMap;

//Light properties
uniform DirectionalLight dirLight;
uniform PointLight[NUM_POINT] pointLights;
uniform SpotLight[NUM_SPOT] spotLights;
uniform vec3 ambient;
uniform int numDirLights;
uniform int numPointLights;
uniform int numSpotLights;

void calcDirectional();
void calcPointLight();
void calcSpotLight();

//Reused variables
int i;
vec3 norm;
vec3 viewDir;
vec3 diffuseTex;
vec3 specularTex;
vec3 c;

void main(){
	//Sort out variables that get reused a lot
	norm = texture(normalMap, texCoords).rgb;
	norm = normalize(norm * 2.0 - 1.0); //Keep in correct range
	norm = normalize(TBN * norm);
	if(textureSize(normalMap, 0).x == 0){
		norm = vec3(0.0,0.0,1.0);
	}
	viewDir = normalize(tangentView - tangentPosition); //Move to vert?
	diffuseTex = texture(diffuse, texCoords).rgb;
	specularTex = texture(specular, texCoords).rgb;
	//Add ambient
	c = ambient * diffuseTex;
	//Add directional light source
	if(numDirLights==1){
		calcDirectional();
	}
	//Add point light sources
	for(i = 0; i < numPointLights; i++){
		calcPointLight();
	}
	//Add spotlight sources
	for(i = 0; i < numSpotLights; i++){
		calcSpotLight();
	}
	//Add emissive light
	c += texture(emissionMap, texCoords).rgb;
	color = vec4(c, 1.0);
	////"Fullbright" debug mode
	//color = texture(diffuse, texCoords).rgba;
	////"NormalMap" debug mode
	//color = vec4(norm, 1.0);
}

void calcDirectional(){
	//Calculate directions
	vec3 lightD = normalize(TBN * dirLight.direction);
	vec3 halfDir = normalize(lightD + viewDir);
	//Get diffuse colours
	float diff = clamp(dot(norm, lightD), 0.0, 1.0);
	vec3 d = dirLight.diffuse * diff * diffuseTex;
	//Get specular colours
	float spec = max(0, pow(max(dot(norm, halfDir), 0.0), shininess));
	vec3 s = dirLight.specular * spec * specularTex;
	//Combine colours
	c += d + s;
	//c = (lightD + 1.0) * 0.5;
}

void calcPointLight(){
	//Calculate directions
	vec3 lightD = TBN * pointLights[i].position - tangentPosition;
	float dist = length(lightD);
	lightD = normalize(lightD);
	vec3 halfDir = normalize(lightD + viewDir);
	//Get diffuse colours
	float diff = max(dot(norm, lightD), 0.0);
	vec3 d = pointLights[i].diffuse * diff * diffuseTex;
	//Get specular colours
	float spec = max(0, pow(max(dot(norm, halfDir), 0.0), shininess));
	vec3 s = pointLights[i].specular * spec * specularTex;
	//Combine colours
	float att = 1.0 / (pointLights[i].constant + pointLights[i].linear * dist +
	            pointLights[i].quadratic * (dist * dist));
	c += (d + s) * att;
}

void calcSpotLight(){
	vec3 lightD = TBN * spotLights[i].position - tangentPosition;
	float dist = length(lightD);
	lightD = normalize(lightD);
	//Calculate cone
	float theta = dot(lightD, normalize(-(TBN * spotLights[i].direction)));
	if(theta > spotLights[i].outerCutOff){
		float epsilon = spotLights[i].cutOff - spotLights[i].outerCutOff;
		float intensity = clamp((theta - spotLights[i].outerCutOff) / epsilon, 0.0, 1.0);
		//Calculate attenuation
		float att = 1.0 / (spotLights[i].constant + spotLights[i].linear * dist +
	            spotLights[i].quadratic * (dist * dist));
		//Calculate directions
		vec3 halfDir = normalize(lightD + viewDir);
		//Get diffuse colours
		float diff = max(dot(norm, lightD), 0.0);
		vec3 d = spotLights[i].diffuse * diff * diffuseTex;
		//Get specular colours
		float spec = max(0, pow(max(dot(norm, halfDir), 0.0), shininess));
		vec3 s = spotLights[i].specular * spec * specularTex;
		//Combine colours
		c += (d * intensity + s * intensity) * att;
	}
}
