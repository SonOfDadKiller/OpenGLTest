#version 330 core

//Uniforms
uniform vec3 color;

//Output
out vec4 FragColor;

void main()
{
	FragColor = vec4(color, 1.0);
}
