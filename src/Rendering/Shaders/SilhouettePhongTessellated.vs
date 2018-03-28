#version 430 core
layout(location = 0) in vec3 vModelSpacePos;
layout(location = 1) in vec2 vTexCoordIn;
layout(location = 2) in vec3 vNormalIn;

out vec3 vPosition;
out vec2 vTexCoord;
out vec3 vNormal;

uniform mat4 modelMat;

void main()
{
	vPosition = (modelMat * vec4(vModelSpacePos, 1)).xyz;
	vTexCoord = vTexCoordIn;
	vNormal = (transpose(inverse(modelMat)) * vec4(vNormalIn, 0)).xyz;
}