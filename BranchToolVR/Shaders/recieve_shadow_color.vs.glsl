#version 410

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform mat4 shadowP;
uniform mat4 shadowV;

out vec3 oi_position;
out vec3 oi_normal;
out vec4 oi_shadow_pos;

void main(){
	gl_Position = P * V * M * vec4(position, 1.0f);
	oi_normal = normalize(vec3(M * vec4(normal, 0.0f)));
	vec4 tmp_pos = M * vec4(position, 1.0f);
	oi_position = vec3(tmp_pos);
	oi_shadow_pos = shadowP *shadowV * tmp_pos;
}
