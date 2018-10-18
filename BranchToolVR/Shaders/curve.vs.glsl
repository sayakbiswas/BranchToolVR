#version 410

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 3) in vec3 instanced_color;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec3 lower_bounds;

out vec3 oi_instanced_color;

void main()
{
	vec3 position = vec3(M * vec4(pos + lower_bounds, 1.0f));
	gl_Position = P * V * vec4(position, 1.0f);
	oi_instanced_color = instanced_color;
}
