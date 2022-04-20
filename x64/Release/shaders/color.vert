#version 330 core

//Vert data
layout (location = 0) in vec3 pos_in;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(pos_in, 1.0);
}