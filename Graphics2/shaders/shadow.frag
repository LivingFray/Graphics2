#version 330 core

//out vec4 c;
layout(location = 0) out float fragmentdepth;

void main(){
	fragmentdepth = gl_FragCoord.z;
	//c = vec4(1.0, 1.0, gl_FragCoord.z, 1.0);
}