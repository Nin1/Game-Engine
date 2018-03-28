#version 430 core

in vec2 texCoord;

out vec4 fragColour;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gEmissive;
uniform sampler2D gShadowMap;

uniform vec3 viewPos;
uniform vec3 cameraViewMat;
uniform mat4 shadowVPMat;
uniform vec3 eyeSpaceLightPos;
uniform vec3 directionalLightColour;
uniform float directionalLightIntensity = 1.0;
uniform vec3 directionalLightEyeDirection;

struct PointLight
{
	vec3 pos;
	vec3 colour;
	float linear;
	float quadratic;
};
const int MAX_POINT_LIGHTS = 32; // See DeferredLightingManager.h
uniform PointLight pointLights[MAX_POINT_LIGHTS];

const float AMBIENT = 0.3;

const vec2 POISSONDISK[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);

// TODO: Remove this function (useful for viewing depth)
float GetLinearShadowMap(in vec2 uv)
{
    float zNear = 0.5;    // TODO: Replace by the zNear of your perspective projection
    float zFar  = 2000.0; // TODO: Replace by the zFar  of your perspective projection
    float depth = texture2D(gShadowMap, uv).x;
    return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
}

void main()
{
	//** GET DATA FROM BUFFER */

    vec3 fragPos = texture(gPosition, texCoord).rgb;
    vec3 normal = normalize(texture(gNormal, texCoord).rgb);
    vec3 albedo = texture(gAlbedoSpec, texCoord).rgb;
    vec3 emissive = texture(gEmissive, texCoord).rgb;
    float specularAmount = texture(gAlbedoSpec, texCoord).a;

	//** APPLY SHADOWS */

	vec4 shadowSpacePos = shadowVPMat * vec4(fragPos, 1.0);
	shadowSpacePos /= shadowSpacePos.w;
	shadowSpacePos = shadowSpacePos * vec4(0.5) + vec4(0.5);
	float dirLightAmount = 1.0;

	for(int i = 0; i < 4; i++)
	{
		float shadowDepth = texture2D(gShadowMap, shadowSpacePos.xy + POISSONDISK[i]/700.0).x;
		dirLightAmount = (shadowSpacePos.z > shadowDepth + 0.01) ? dirLightAmount - 0.25 : dirLightAmount;
	}

    //** DIRECTIONAL LIGHTING */
    
	vec3 lighting;
	vec3 eyeSpaceFragPos = cameraViewMat * fragPos;
	vec3 viewDir = normalize(viewPos - fragPos);
	{
		//vec3 lightDir = normalize(eyeSpaceLightPos - eyeSpaceFragPos);
		vec3 lightDir = normalize(directionalLightEyeDirection);
		vec3 diffuse = max(dot(normal, lightDir), 0) * albedo * directionalLightColour;
		vec3 halfVec = normalize(normalize(eyeSpaceFragPos) + (-lightDir));
		int facing = (dot(normal, lightDir) > 0) ? 1 : 0;
		vec3 specular = vec3(1.0); //directionalLightColour * facing * directionalLightIntensity * pow(max(dot(normal, halfVec), 0), specularAmount);
		lighting = vec3((AMBIENT * albedo) + (dirLightAmount * (emissive + diffuse * specular)));
	}

	//** POINT LIGHTS */

    for(int i = 0; i < MAX_POINT_LIGHTS; ++i)
    {
        // diffuse
        vec3 lightDir = normalize(pointLights[i].pos - fragPos);
        vec3 diffuse = max(dot(normal, lightDir), 0.0) * albedo * pointLights[i].colour;
        // specular
        vec3 halfwayDir = normalize((-lightDir) + normalize(eyeSpaceFragPos));  
        float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
        vec3 specular = pointLights[i].colour * spec * specularAmount;
        // attenuation
        float distance = length(pointLights[i].pos - fragPos);
        float attenuation = 1.0 / (1.0 + pointLights[i].linear * distance + pointLights[i].quadratic * distance * distance);
        diffuse *= attenuation;
        specular *= attenuation;
        lighting += diffuse + specular;        
    }

	//** OUTPUT COLOUR */

    fragColour = vec4(lighting, 1.0);
	//fragColour = texture2D(gShadowMap, texCoord);
}