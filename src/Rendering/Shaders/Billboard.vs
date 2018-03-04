#version 430 core
layout(location = 0) in vec3 vModelSpacePos;
layout(location = 1) in vec2 vTexCoordIn;
layout(location = 2) in vec3 vNormalIn;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

// The center position of the billboard object
uniform vec3 centerWorldPos;
// The scale of the billboard object
uniform vec3 worldScale;

out vec3 fragPos;
out vec3 vNormal;
out vec2 texCoord;

void main()
{
	texCoord = vTexCoordIn;

	// Find camera right and camera up vectors
	vec3 cameraRight = { viewMat[0][0], viewMat[1][0], viewMat[2][0] };
	vec3 cameraUp = { viewMat[0][1], viewMat[1][1], viewMat[2][1] };
	vec3 cameraForward = { viewMat[0][2], viewMat[1][2], viewMat[2][2] };

	// Calculate the world position of this vertex
	vec3 vertexWorldPos =
		centerWorldPos
		+ cameraRight * vModelSpacePos.z * worldScale.z
		+ cameraUp * vModelSpacePos.y * worldScale.x;

	gl_Position = projMat * viewMat * vec4(vertexWorldPos, 1);
	fragPos = vertexWorldPos;
	vNormal = -cameraForward;
	modelMat;
}
