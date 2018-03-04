#version 430 core
layout (location = 0) out vec3 fPosition;
layout (location = 1) out vec3 fNormal;
layout (location = 2) out vec4 fAlbedoSpec;
layout (location = 3) out vec3 fEmissive;

in vec3 gFragPos;
in vec3 gNormal;
in vec2 gTexCoord;

uniform sampler2D tex1;

void main()
{
	// Fragment position vector
	fPosition = gFragPos;
	// Fragment normal
	fNormal = gNormal;
	// Albedo (texture colour)
	fAlbedoSpec.rgb = texture(tex1, gTexCoord).rgb;
	// Specular intensity
	fAlbedoSpec.a = 0.5;
	// Emissive
	fEmissive = vec3(1.0, 0.0, 0.0);
}