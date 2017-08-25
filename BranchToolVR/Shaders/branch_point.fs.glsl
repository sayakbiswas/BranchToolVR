#version 410 core

in vec3 oi_pos;
in vec3 oi_normal;
in float oi_lighting;

uniform vec4 color;
vec3 ambient = vec3(0.5f,0.5f,0.5f);
out vec4 output_color;

void main()
{
	output_color = vec4(vec3(color)*oi_lighting + ambient*vec3(color),1.0f);
}
    