#version 330 core

//Vert data
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 vert_localPos;

void main()
{
	//gl_Position = projection * view * model * vec4(in_pos, 1.0);
	vec3 pos = in_pos + in_normal * 0.1;
	//vec3 pos = in_pos * 1.1;
	gl_Position = projection * view * model * vec4(pos, 1.0);
	vert_localPos = in_pos;
}
