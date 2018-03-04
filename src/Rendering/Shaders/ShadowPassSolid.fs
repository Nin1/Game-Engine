#version 430 core
layout (location = 0) out float gDepth;

void main()
{
	gDepth = gl_FragCoord.z;
	gDepth = 0.0;
}