#version 330
layout (location = 0) in vec3 vertexPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

out vec2 screenSize;

float screenWidth = 1280;
float screenHeight = 720;

void main(){
	gl_Position = lightSpaceMatrix * model * vec4(vertexPos, 1.0);
	screenSize = vec2(screenWidth, screenHeight);
}