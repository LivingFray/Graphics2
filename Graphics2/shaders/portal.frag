#version 330

in vec2 screenSize;

out vec4 fragColour;

uniform sampler2D portal;

void main(){
	fragColour = texture(portal, gl_FragCoord.xy / screenSize);
}