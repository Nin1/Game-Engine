#version 430 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 fragPos;
in vec3 vNormal;
in vec2 texCoord;

uniform sampler2D tex1;

void main()
{
	// Fragment position vector
	gPosition = fragPos;
	// Fragment normal
	gNormal = vNormal;
	// Albedo (texture colour)
	gAlbedoSpec.rgb = texture(tex1, texCoord).rgb;
	// Specular intensity
	gAlbedoSpec.a = 0.5;
}