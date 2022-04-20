#version 330 core

//Input from Vert Shader
in vec2 vert_uv;
in vec3 vert_normal;
in vec3 vert_worldPos;

//Uniforms
uniform sampler2D main_tex;
uniform vec3 color;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

//Output
out vec4 frag_color;

void main()
{
	//Calculate ambient
	vec3 ambient = lightColor * 0.6;

	//Calculate diffuse
	vec3 _normal = normalize(vert_normal);
	vec3 lightDir = normalize(lightPos - vert_worldPos);
	float litAmount = max(dot(_normal, lightDir), 0);
	vec3 diffuse = litAmount * lightColor * 2;

	//Calculate specular
	vec3 viewDir = normalize(viewPos - vert_worldPos);
	vec3 reflectDir = reflect(-lightDir, _normal);

	float specAmount = pow(max(dot(viewDir, reflectDir), 0.0), 64);
	vec3 specular = specAmount * lightColor * 0.1;


	vec3 lighting = color * (ambient + diffuse + specular);

	frag_color = texture(main_tex, vert_uv) * vec4(lighting, 1.0);
}
