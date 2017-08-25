#version 410 core

in vec3 oi_position;
in vec3 oi_normal;
in vec4 oi_shadow_pos;

out vec4 output_color;

struct Light
{
  vec3 pos;
  float pow;
};

uniform vec4 color;
uniform Light lights[3];
uniform sampler2D shadow;
uniform vec3 ambient;

vec3 light = vec3(10.0f,10.0f,10.0f);
float ShadowCalculation(vec4 fragPosLightSpace)
{
	// perform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

	// Transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;

	// Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	float closestDepth = texture(shadow, projCoords.xy).r; 
	//texelFetch(depthTex, ivec2(tmp), i)
	//float closestDepth =texelFetch(shadow, projCoords.xy, 8).r;

	// Get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;

	// Calculate bias (based on depth map resolution and slope)
	vec3 normal = normalize(oi_normal);
	vec3 lightDir = normalize(light - oi_position);
	float bias = max(0.002f * (1.0 - dot(normal, lightDir)), 0.002f);
	//bias = 0.001f;

	//return currentDepth - bias > closestDepth  ? 1.0 : 0.0;
	// Check whether current frag pos is in shadow
	 float shadow1 = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
	// PCF
	//float shadow1 = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadow, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadow, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow1 += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
		}    
	}

	shadow1 /= 9.0;

	// Keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
	if(projCoords.z > 1.0)
		shadow1 = 0.0;
    
	return shadow1*1.0f;
}

 
void main()
{
	float light_res  = 0.5f*clamp(abs(dot(oi_normal, normalize(lights[0].pos - oi_position)))/length(lights[0].pos - oi_position), 0.0f, 1.0f);
	      light_res += 0.5f*clamp(abs(dot(oi_normal, normalize(lights[1].pos - oi_position)))/length(lights[1].pos - oi_position), 0.0f, 1.0f);
	      light_res += 0.5f*clamp(abs(dot(oi_normal, normalize(lights[2].pos - oi_position)))/length(lights[2].pos - oi_position), 0.0f, 1.0f);
	float shadow_res = 1.0f - ShadowCalculation(oi_shadow_pos);
	output_color = vec4(shadow_res*light_res*color.xyz + ambient, color.w);
}
    