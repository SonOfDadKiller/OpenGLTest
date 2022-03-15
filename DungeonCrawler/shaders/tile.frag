#version 330 core

//Input from Vert Shader
in vec2 uv_vert;

//Uniforms
uniform sampler2D main_tex;
uniform vec4 color;

//Output
out vec4 FragColor;

void main()
{
	FragColor = texture(main_tex, uv_vert) * color;
	//FragColor = vec4(uv_vert, 0.0, 1.0);
}
