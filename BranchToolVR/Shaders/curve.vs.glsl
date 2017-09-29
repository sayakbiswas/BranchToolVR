#version 410

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

void main()
{
	gl_Position = P * V * M * vec4(pos, 1.f);
}