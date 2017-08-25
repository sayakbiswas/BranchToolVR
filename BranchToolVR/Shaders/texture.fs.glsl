#version 410 core

in vec3 oi_position;
in vec3 oi_normal;
in vec2 oi_uv;

out vec4 output_color;

uniform sampler2D diffuse_texture;
uniform vec3 ambient;

vec3 light_pos = vec3(1.0f,1.0f,1.0f);

void main()
{
	//vec3 to_light = normalize(light_pos - oi_position);
	output_color = texture(diffuse_texture, oi_uv);
	//output_color.a = 0.5f;
}
