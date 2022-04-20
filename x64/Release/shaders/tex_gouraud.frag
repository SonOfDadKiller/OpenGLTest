#version 330 core

//Input from Vert Shader
in vec2 vert_uv;
in vec3 vert_color;

//Uniforms
uniform sampler2D main_tex;

//Output
out vec4 frag_color;


void main()
{
	frag_color = texture(main_tex, vert_uv) * vec4(vert_color, 1.0);
}
