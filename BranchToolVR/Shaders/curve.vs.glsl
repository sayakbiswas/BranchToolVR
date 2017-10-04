#version 410

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 instanced_position;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec3 lower_bounds;
uniform vec3 instanced_position;

void main()
{
	gl_Position = P * V * M * vec4(pos + lower_bounds + instanced_position, 1.f);
}