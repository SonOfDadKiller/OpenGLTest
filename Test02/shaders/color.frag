#version 330 core

//input
in vec3 vert_localPos;

//Uniforms
//uniform vec3 color;

//Output
out vec4 frag_color;

void main()
{
	frag_color = vec4(vert_localPos, 1.0);
}