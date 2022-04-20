#version 330 core

//Vert data
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;

void main()
{
	gl_Position = vec4(in_pos * 0.1, 1.0);
}
