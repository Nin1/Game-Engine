#version 430 core
layout(location = 0) in vec3 vPosIn;
layout(location = 1) in vec2 vTexCoordIn;

out vec2 texCoord;

void main()
{
	texCoord = vTexCoordIn;
	gl_Position = vec4(vPosIn, 1.0);
}