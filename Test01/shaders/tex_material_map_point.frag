#version 330 core

//Input from Vert Shader
in vec2 vert_uv;
in vec3 vert_normal;
in vec3 vert_worldPos;

//Uniforms
uniform vec3 viewPos;

struct Material
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	float shininess;
};

struct Light
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float attenConstant;
	float attenLinear;
	float attenQuadratic;
};

uniform Material material;
uniform Light light;

//Output
out vec4 frag_color;

void main()
{
	float lightDistance = length(light.position - vert_worldPos);
	float attenuation = 1.0 / (light.attenConstant + light.attenLinear * lightDistance + 
				light.attenQuadratic * (lightDistance * lightDistance));

	//Calculate diffuse
	vec3 normal = normalize(vert_normal);
	vec3 lightDir = normalize(light.position - vert_worldPos);
	float litAmount = max(dot(normal, lightDir), 0);
	vec3 diffuseSample = texture(material.diffuseMap, vert_uv).xyz;
	vec3 diffuse = litAmount * light.diffuse * diffuseSample * attenuation;

	//Calculate ambient
	vec3 ambient = light.ambient * attenuation * diffuseSample;

	//Calculate specular
	vec3 viewDir = normalize(viewPos - vert_worldPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float specAmount = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specularSample = texture(material.specularMap, vert_uv).xyz;
	vec3 specular = specAmount * light.specular  * attenuation * specularSample;

	vec3 lighting = ambient + diffuse + specular;
	
	frag_color = vec4(lighting, 1.0);
}
