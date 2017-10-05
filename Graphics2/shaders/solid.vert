#version 330
layout(location = 0) in vec3 vertexPosition;

out vec4 colour;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
	gl_Position = projection  * view * model * vec4(vertexPosition, 1.0);
	colour = vec4(1.0, 1.0, 1.0, 1.0);
}