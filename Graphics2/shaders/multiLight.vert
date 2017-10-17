#version 330 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBitangent;

out vec3 tangentPosition;
out vec2 texCoords;
out vec3 tangentView;
out mat3 TBN;

uniform mat4 model;
uniform mat3 transInvModel; //Used to correct normals
uniform mat4 view;
uniform mat4 projection;

uniform vec3 viewPos;

void main(){
	//Pass texture coords to fragment shader
	texCoords = vertexUV;
	gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
	//Calculate TBN
	vec3 T = normalize(vec3(transInvModel * vertexTangent));
	vec3 B = normalize(vec3(transInvModel * vertexBitangent));
	vec3 N = normalize(vec3(transInvModel * vertexNormal));
	TBN = transpose(mat3(T, B, N));
	tangentView = TBN * viewPos;
	tangentPosition = TBN * vec3(model * vec4(vertexPosition, 1.0));
}
