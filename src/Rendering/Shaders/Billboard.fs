#version 430 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec3 gEmissive;

in vec3 fragPos;
in vec3 vNormal;
in vec2 texCoord;

uniform sampler2D albedo;
uniform sampler2D normal;
uniform bool useNormalMap;

void main()
{
	vec4 tex = texture(albedo, texCoord);

	if(tex.a == 0)
	{
		discard;
	}

	// Fragment position vector
	gPosition = fragPos;
	// Fragment normal
	if(useNormalMap)
	{
		gNormal = vNormal + texture(normal, texCoord).rgb;
	}
	else
	{
		gNormal = vNormal;
	}
	// Albedo (colour)
	gAlbedoSpec.rgb = tex.rgb;
	// Specular intensity
	gAlbedoSpec.a = 0;
	// Emissive
	gEmissive.rgb = vec3(0.0, 0.0, 0.0);
}