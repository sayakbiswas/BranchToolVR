#version 410

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

out vec3 oi_position;
out vec3 oi_normal;
out vec2 oi_uv;

void main(){
	oi_normal = normalize(vec3(M * vec4(normal, 0.0f)));
	oi_position = vec3( M * vec4(position, 1.0f));
	oi_uv = uv;
	gl_Position = P * V * M * vec4(position, 1.0f);
}
