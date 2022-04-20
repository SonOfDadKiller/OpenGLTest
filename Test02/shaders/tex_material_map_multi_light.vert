#version 330 core

//Vert data
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

//Output
out vec2 vert_uv;
out vec3 vert_normal;
out vec3 vert_worldPos;

void main()
{
	gl_Position = projection * view * model * vec4(in_pos, 1.0);
	vert_uv = in_uv;
	vert_normal = mat3(transpose(inverse(model))) * in_normal;
	vert_worldPos = vec3(model * vec4(in_pos, 1.0));
}
