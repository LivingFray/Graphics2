#version 330 core
//Hopefully this will be a temporary shader and I will get around to deferred shading
out vec4 color;

in vec3 tangentPosition;
in vec2 texCoords;
//in vec3 worldNormal;
in vec3 lightPos;
in vec3 lightDir;
in vec3 tangentView;

struct DirectionalLight {
	vec3 direction;
};

struct PointLight {
	vec3 position;
	//Attenuation
	float constant;
	float linear;
	float quadratic;
};

struct SpotLight {
	vec3 position;
	vec3 direction;
	//
	float cutOff;
	float outerCutOff;
	//Attenuation
	float constant;
	float linear;
	float quadratic;
};

//Shared properties
struct AllLights {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

//Material properties
uniform sampler2D diffuse;
uniform sampler2D specular;
uniform float shininess;
uniform sampler2D normalMap;
uniform sampler2D emissionMap;

//Light properties
uniform int lightType; //0=DirectionalLight, 1=PointLight, 2=SpotLight
uniform DirectionalLight dirLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;
uniform AllLights light;

void calcDirectional();
void calcPointLight();
void calcSpotLight();

void main(){
	color = vec4(0.0, 0.0, 0.0, 0.0);
	if(lightType == 0){
		calcDirectional();
	} else if(lightType == 1) {
		calcPointLight();
	} else if(lightType == 2) {
		calcSpotLight();
	}
	color = color + vec4(texture(emissionMap, texCoords).rgb, 1.0);
	//"Fullbright" debug mode
	//color = texture(diffuse, texCoords).rgba;
}

void calcDirectional(){
	//Calculate directions
	vec3 lightD = normalize(-lightDir);
	//vec3 norm = normalize(worldNormal);
	vec3 norm = texture(normalMap, texCoords).rgb;
	norm = normalize(norm * 2.0 - 1.0); //Keep in correct range
	vec3 viewDir = normalize(tangentView - tangentPosition);
	vec3 halfDir = normalize(lightD + viewDir);
	//Textures used to get material properties
	vec3 diffuseTex = texture(diffuse, texCoords).rgb;
	vec3 specularTex = texture(specular, texCoords).rgb;
	//Get ambient colours
	vec3 a = light.ambient * diffuseTex;
	//Get diffuse colours
	float diff = max(dot(norm, lightD), 0.0);
	vec3 d = light.diffuse * diff * diffuseTex;
	//Get specular colours
	float spec = max(0, pow(max(dot(norm, halfDir), 0.0), shininess));
	vec3 s = light.specular * spec * specularTex;
	//Combine 3 colours
	color = vec4(a + d + s, 1.0);//vec4(a + d + s, 1.0);
}

void calcPointLight(){
	//Calculate directions
	vec3 lightD = normalize(lightPos - tangentPosition);
	//vec3 norm = normalize(worldNormal);
	vec3 norm = texture(normalMap, texCoords).rgb;
	norm = normalize(norm * 2.0 - 1.0); //Keep in correct range
	vec3 viewDir = normalize(tangentView - tangentPosition);
	vec3 halfDir = normalize(lightD + viewDir);
	//Textures used to get material properties
	vec3 diffuseTex = texture(diffuse, texCoords).rgb;
	vec3 specularTex = texture(specular, texCoords).rgb;
	//Get ambient colours
	vec3 a = light.ambient * diffuseTex;
	//Get diffuse colours
	float diff = max(dot(norm, lightD), 0.0);
	vec3 d = light.diffuse * diff * diffuseTex;
	//Get specular colours
	float spec = max(0, pow(max(dot(norm, halfDir), 0.0), shininess));
	vec3 s = light.specular * spec * specularTex;
	//Combine 3 colours
	float dist = length(lightPos - tangentPosition);
	float att = 1.0 / (pointLight.constant + pointLight.linear * dist +
	            pointLight.quadratic * (dist * dist));
	color = vec4((a + d + s) * att, 1.0);
}

void calcSpotLight(){
	vec3 lightD = normalize(lightPos - tangentPosition);
	//Calculate cone
	float theta = dot(lightD, normalize(-lightDir));
	float epsilon = spotLight.cutOff - spotLight.outerCutOff;
	float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);
	vec3 diffuseTex = texture(diffuse, texCoords).rgb;
	//Calculate attenuation
	float dist = length(lightPos - tangentPosition);
	float att = 1.0 / (spotLight.constant + spotLight.linear * dist +
	            spotLight.quadratic * (dist * dist));
	if(theta > spotLight.outerCutOff){
		//Calculate directions
		//vec3 norm = normalize(worldNormal);
		vec3 norm = texture(normalMap, texCoords).rgb;
		norm = normalize(norm * 2.0 - 1.0); //Keep in correct range
		vec3 viewDir = normalize(tangentView - tangentPosition);
		vec3 halfDir = normalize(lightD + viewDir);
		//Textures used to get material properties
		vec3 specularTex = texture(specular, texCoords).rgb;
		//Get ambient colours
		vec3 a = light.ambient * diffuseTex;
		//Get diffuse colours
		float diff = max(dot(norm, lightD), 0.0);
		vec3 d = light.diffuse * diff * diffuseTex;
		//Get specular colours
		float spec = max(0, pow(max(dot(norm, halfDir), 0.0), shininess));
		vec3 s = light.specular * spec * specularTex;
		//Combine 3 colours
		color = vec4((a + d * intensity + s * intensity) * att, 1.0);
	}else{
		//Don't affect lighting
		color = vec4(0.0, 0.0, 0.0, 0.0);
		//color = vec4(light.ambient * diffuseTex * att, 1.0);
	}
}
