#version 410

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

struct Light{
  vec3 pos;
  float pow;
};

uniform Light lights[3];

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec3 instanced_position;
uniform vec3 lower_bounds;
uniform vec3 scale;

out vec3 oi_pos;
out vec3 oi_normal;
out float oi_lighting;

void main()
{
	vec4 tmp = M * vec4(position + instanced_position + lower_bounds, 1.0f);
	gl_Position = P * V * tmp;
	oi_normal = normalize(vec3(M * vec4(normal, 0.0f)));
	oi_pos = vec3(tmp);

	vec3 to_light1 = lights[0].pos - oi_pos;
	vec3 to_light2 = lights[1].pos - oi_pos;
	vec3 to_light3 = lights[2].pos - oi_pos;

	float light_dist1 = length(to_light1);
	float light_dist2 = length(to_light2);
	float light_dist3 = length(to_light3);

	oi_lighting  =  0.5f*clamp(dot(oi_normal, normalize(to_light1)),0.0f, 1.0f)/(light_dist1*light_dist1);
	oi_lighting +=  0.5f*clamp(dot(oi_normal, normalize(to_light2)),0.0f, 1.0f)/(light_dist2*light_dist2);
	oi_lighting +=  0.5f*clamp(dot(oi_normal, normalize(to_light3)),0.0f, 1.0f)/(light_dist3*light_dist3);
}
