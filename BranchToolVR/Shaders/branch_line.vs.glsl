#version 410

uniform vec3 pos1;
uniform vec3 pos2;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec3 scale;
uniform vec3 lower_bounds;

void main()
{
	if(gl_VertexID == 0)
	{
		gl_Position = P * V * M*vec4((pos1 - lower_bounds), 1.0f);
	}
	else
	{
		gl_Position = P * V * M*vec4((pos2 - lower_bounds), 1.0f);
	}
}
