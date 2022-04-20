#version 330 core

//Input from Vert Shader
in vec2 vert_uv;
in vec3 vert_normal;
in vec3 vert_worldPos;

//Uniforms
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

struct Material
{
	sampler2D diffuseMap;
	sampler2D specularMap;
	float shininess;
};

struct Light
{
	vec3 direction;
	float cutoff;
	float cutoffInner;
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
	vec3 lightDir = normalize(lightPos - vert_worldPos);

	//Calculate spotlight intensity
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutoffInner - light.cutoff;
	float spot = clamp((theta - light.cutoff) / epsilon, 0.0, 1.0);
	
	//Calculate light attenuation
	float lightDistance = length(lightPos - vert_worldPos);
	float attenuation = 1.0 / (light.attenConstant + light.attenLinear * lightDistance + 
				light.attenQuadratic * (lightDistance * lightDistance));

	//Calculate diffuse
	vec3 normal = normalize(vert_normal);
	float litAmount = max(dot(normal, lightDir), 0);
	vec3 diffuseSample = texture(material.diffuseMap, vert_uv).xyz;
	vec3 diffuse = litAmount * light.diffuse * diffuseSample * attenuation * spot;

	//Calculate ambient
	vec3 ambient = light.ambient * attenuation * diffuseSample;

	//Calculate specular
	vec3 viewDir = normalize(viewPos - vert_worldPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float specAmount = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specularSample = texture(material.specularMap, vert_uv).xyz;
	vec3 specular = specAmount * light.specular  * attenuation * spot * specularSample;

	vec3 lighting = ambient + diffuse + specular;
	
	frag_color = vec4(lighting, 1.0);
}
