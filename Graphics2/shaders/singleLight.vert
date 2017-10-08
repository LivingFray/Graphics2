#version 330 core
//Hopefully this will be a temporary shader and I will get around to deferred shading
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBitangent;

out vec3 tangentPosition;
out vec2 texCoords;
//out vec3 worldNormal;
out vec3 tangentView;
out vec3 lightPos;
out vec3 lightDir;

//Lighting
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

uniform int lightType; //0=DirectionalLight, 1=PointLight, 2=SpotLight
uniform DirectionalLight dirLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;
uniform mat4 model;
uniform mat3 transInvModel; //Used to correct normals
uniform mat4 view;
uniform mat4 projection;

uniform vec3 viewPos;

void main(){
	//Convert position to model space
	//tangentPosition = vec3(model * vec4(vertexPosition, 1.0));
	//Convert normal to model space
	//worldNormal = transInvModel * vertexNormal;
	//Pass texture coords to fragment shader
	texCoords = vertexUV;
	gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
	//Calculate TBN
	vec3 T = normalize(vec3(transInvModel * vertexTangent));
	vec3 B = normalize(vec3(transInvModel * vertexBitangent));
	vec3 N = normalize(vec3(transInvModel * vertexNormal));
	mat3 TBN = transpose(mat3(T, B, N));
	tangentView = TBN * viewPos;
	if(lightType == 0){
		lightDir = TBN * dirLight.direction;
	}else if(lightType == 1){
		lightPos = TBN * pointLight.position;
	}else if(lightType == 2){
		lightPos = TBN * spotLight.position;
		lightDir = TBN * spotLight.direction;
	}
	tangentPosition = TBN * vec3(model * vec4(vertexPosition, 1.0));
}
