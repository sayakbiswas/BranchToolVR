#version 410

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

out vec3 oi_color;

void main(){
	gl_Position = P * V * M * vec4(position, 1.0f);
	oi_color = color;
}
