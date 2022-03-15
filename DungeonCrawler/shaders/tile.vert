#version 330 core

//Vert data
layout (location = 0) in vec3 pos_in;
layout (location = 1) in vec2 uv_in;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

//Output
out vec2 uv_vert;

void main()
{
	gl_Position = projection * view * model * vec4(pos_in, 1.0);
	uv_vert = uv_in;
}
