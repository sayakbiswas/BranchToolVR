#version 410 core

in float oi_lighting;
in vec3 oi_instanced_color;

out vec4 output_color;

uniform vec3 ambient;

void main()
{
	output_color = vec4(oi_lighting*oi_instanced_color + ambient, 1.0f);
	//outputColor = vec4(oi_lighting*oi_instanced_color + vec3(0.5f,0.5f,0.5f), 1.0f);
	//outputColor = vec4(vec3(oi_lighting,oi_lighting,oi_lighting), 1.0f);
	//output_color = vec4(oi_instanced_color,1.0f);
}