#version 330 core

//Input from Vert Shader
in vec2 vert_uv;
in vec3 vert_normal;
in vec3 vert_worldPos;

//Uniforms
uniform sampler2D texture_diffuse1;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
  
uniform Material material;
uniform Light light;  

//Output
out vec4 frag_color;

void main()
{
	//Calculate ambient
	vec3 ambient = light.ambient * material.ambient;

	//Calculate diffuse
	vec3 normal = normalize(vert_normal);
	vec3 lightDir = normalize(lightPos - vert_worldPos);
	float litAmount = max(dot(normal, lightDir), 0);
	vec3 diffuse = litAmount * light.diffuse * material.diffuse;

	//Calculate specular
	vec3 viewDir = normalize(viewPos - vert_worldPos);
	vec3 reflectDir = reflect(-lightDir, normal);

	float specAmount = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = specAmount * light.specular * material.specular;

	vec3 lighting = ambient + diffuse + specular;

	//frag_color = texture(texture_diffuse1, vert_uv);// * vec4(lighting, 1.0);
	frag_color = vec4(1.0);
}
