#version 330 core
out vec4 color;

in vec3 texCoords;

uniform samplerCube skybox;
uniform vec3 skyColour;
uniform float skyAmount;

void main(){
	vec3 sunPos = vec3(0.0, 1.0, 0.0);
	vec4 sunColour = vec4(1.0, 1.0, 0.4, 1.0);
	float sunSize = 0.025;
	float falloff = 20.0;
	float sunAmount = length(texCoords - sunPos) < sunSize ? 1.0 : 1.0 - clamp((length(texCoords - sunPos) - sunSize) * falloff, 0.0, 1.0);
	color = texture(skybox, texCoords) * (1.0 - skyAmount) + skyAmount * vec4(skyColour, 1.0);
	color = color * (1.0 - sunAmount) + sunAmount * sunColour;
}