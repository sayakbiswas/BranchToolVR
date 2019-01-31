#version 410 core

in float oi_lighting;
in vec4 oi_instanced_color;

out vec4 output_color;

uniform vec4 ambient;

void main()
{
	output_color = vec4(oi_lighting*oi_instanced_color + ambient);
	output_color.a = oi_instanced_color.a;
	//outputColor = vec4(oi_lighting*oi_instanced_color + vec3(0.5f,0.5f,0.5f), 1.0f);
	//outputColor = vec4(vec3(oi_lighting,oi_lighting,oi_lighting), 1.0f);
	//output_color = vec4(oi_instanced_color,1.0f);
}