#version 330 core

layout (location = 0) in vec3 pos;

out vec3 texCoords;

uniform mat4 projection;
uniform mat4 view;

void main(){
	texCoords = vec3(pos.x, pos.y, -pos.z);
	gl_Position = (projection * view * vec4(pos, 1.0)).xyww;
}