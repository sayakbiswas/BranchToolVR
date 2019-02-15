#version 410

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 instanced_position;
layout(location = 3) in vec4 instanced_color;
layout(location = 4) in float instanced_iso_diff;

struct Light
{
  vec3 pos;
  float power;
};

uniform Light lights[3];

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform float tolerance;
uniform vec3 scale;

out float oi_lighting;
out vec4 oi_instanced_color;

void main()
{
	vec3 pos = vec3(M*vec4(position + instanced_position, 1.0f));
	vec3 nor = normalize(vec3(M*vec4(normal, 0.0f)));
	//oi_instanced_color = vec3(0.5f,1.0f,0.1f);
	oi_instanced_color = instanced_color;

	vec3 to_light1 = lights[0].pos - pos;
	vec3 to_light2 = lights[1].pos - pos;
	vec3 to_light3 = lights[2].pos - pos;

	float light_dist1 = length(to_light1);
	float light_dist2 = length(to_light2);
	float light_dist3 = length(to_light3);

	oi_lighting  =  0.5f*clamp(dot(nor, normalize(to_light1)),0.0f, 1.0f)/(light_dist1*light_dist1);
	oi_lighting +=  0.5f*clamp(dot(nor, normalize(to_light2)),0.0f, 1.0f)/(light_dist2*light_dist2);
	oi_lighting +=  0.5f*clamp(dot(nor, normalize(to_light3)),0.0f, 1.0f)/(light_dist3*light_dist3);

	gl_Position = P * V * vec4(pos, 1.0f); // + max(sign(instanced_iso_diff - tolerance), 0.0) * vec4(100000.0f,100000.0f,100000.0f,100000.0f);
}
