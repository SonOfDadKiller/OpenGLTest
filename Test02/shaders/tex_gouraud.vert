#version 330 core

//Vert data
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 color;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

//Output
out vec2 vert_uv;
out vec3 vert_color;

void main()
{
	//Calculate ambient
	vec3 ambient = lightColor * 0.6;

	//Calculate diffuse
	vec3 normal = mat3(transpose(inverse(model))) * normalize(in_normal);
	vec3 worldPos = vec3(model * vec4(in_pos, 1.0));
	vec3 lightDir = normalize(lightPos - worldPos);
	float litAmount = max(dot(normal, lightDir), 0);
	vec3 diffuse = litAmount * lightColor * 2;

	//Calculate specular
	vec3 viewDir = normalize(viewPos - worldPos);
	vec3 reflectDir = reflect(-lightDir, normal);

	float specAmount = pow(max(dot(viewDir, reflectDir), 0.0), 64);
	vec3 specular = specAmount * lightColor * 0.1;

	vert_color = color * (ambient + diffuse + specular);
	vert_uv = in_uv;

	gl_Position = projection * view * model * vec4(in_pos, 1.0);
}
