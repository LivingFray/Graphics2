#version 330 core
out vec4 color;

in vec3 texCoords;

uniform samplerCube skybox;
uniform vec3 skyColour;
uniform float skyAmount;

void main(){
	color = texture(skybox, texCoords) * (1.0 - skyAmount) + skyAmount * vec4(skyColour, 1.0);
}