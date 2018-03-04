#version 430 core
layout(location = 0) in vec3 vModelSpacePos;

uniform mat4 depthMVP;

void main()
{
	gl_Position = depthMVP * vec4(vModelSpacePos, 1);
}