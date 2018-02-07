#version 430 core
layout(location = 0) in vec3 vModelSpacePos;
layout(location = 1) in vec3 vNormalIn;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

out vec3 fragPos;
out vec3 vNormal;

void main()
{
	vec4 worldPos = modelMat * vec4(vModelSpacePos, 1);
	gl_Position = projMat * viewMat * worldPos;
	fragPos = worldPos.rgb;
	vNormal = normalize(mat3(transpose(inverse(modelMat))) * vNormalIn);
}