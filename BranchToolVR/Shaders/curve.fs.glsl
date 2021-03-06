#version 410 core

in vec3 oi_instanced_color;

out vec4 output_color;

uniform vec3 ambient;

void main()
{
	output_color = vec4(oi_instanced_color + ambient, 1.0f);
}
