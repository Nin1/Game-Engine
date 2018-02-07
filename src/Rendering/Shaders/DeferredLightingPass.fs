#version 430 core

in vec2 texCoord;

out vec4 fragColour;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct PointLight
{
	vec3 pos;
	vec3 colour;
	float linear;
	float quadratic;
};
const int MAX_POINT_LIGHTS = 32; // See DeferredLightingManager.h
uniform PointLight pointLights[MAX_POINT_LIGHTS];

uniform vec3 viewPos;

const float AMBIENT = 0.1;

void main()
{
    // retrieve data from gbuffer
    vec3 fragPos = texture(gPosition, texCoord).rgb;
    vec3 normal = texture(gNormal, texCoord).rgb;
    vec3 albedo = texture(gAlbedoSpec, texCoord).rgb;
    float specularAmount = texture(gAlbedoSpec, texCoord).a;
    
    // then calculate lighting as usual
    vec3 lighting  = albedo * AMBIENT; // hard-coded ambient component
    vec3 viewDir  = normalize(viewPos - fragPos);
    for(int i = 0; i < MAX_POINT_LIGHTS; ++i)
    {
        // diffuse
        vec3 lightDir = normalize(pointLights[i].pos - fragPos);
        vec3 diffuse = max(dot(normal, lightDir), 0.0) * albedo * pointLights[i].colour;
        // specular
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
        vec3 specular = pointLights[i].colour * spec * specularAmount;
        // attenuation
        float distance = length(pointLights[i].pos - fragPos);
        float attenuation = 1.0 / (1.0 + pointLights[i].linear * distance + pointLights[i].quadratic * distance * distance);
        diffuse *= attenuation;
        specular *= attenuation;
        lighting += diffuse + specular;        
    }
    fragColour = vec4(lighting, 1.0);
}