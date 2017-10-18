#version 330 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 transInvModel;

out mat3 TBN;
out vec2 texCoords;
out vec3 fragmentPos;

void main(){
	//Pass tex coords
	texCoords = vertexUV;
	//Standard transformation
	gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
	//Translate coordinates to be in tangent space
	vec3 T = normalize(vec3(transInvModel * vertexTangent));
	vec3 B = normalize(vec3(transInvModel * vertexBitangent));
	vec3 N = normalize(vec3(transInvModel * vertexNormal));
	TBN = mat3(T,B,N);
	//Calculate position of fragment in world space
	fragmentPos = vec3(model * vec4(vertexPosition, 1.0));
}
