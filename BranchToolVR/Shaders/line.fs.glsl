#version 410 core

in vec3 oi_color;

out vec4 output_color;


void main(){
	output_color = vec4(oi_color,1.0f);
}
