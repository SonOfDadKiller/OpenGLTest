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
uniform Material material;

struct DirLight
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform DirLight dirLight;

struct PointLight
{
	vec3 position;
	float attenConstant;
	float attenLinear;
	float attenQuadratic;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

#define NUM_POINT_LIGHTS 4
uniform PointLight pointLights[NUM_POINT_LIGHTS];

//Output
out vec4 frag_color;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);

	//Calculate diffuse
	float litAmount = max(dot(normal, lightDir), 0.0);
	vec3 diffuseSample = texture(material.diffuseMap, vert_uv).xyz;
	vec3 diffuse = light.diffuse * litAmount * diffuseSample;

	//Calculate specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float specAmount = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specSample = texture(material.specularMap, vert_uv).xyz;
	vec3 specular = light.specular * specAmount * specSample;

	//Calculate ambient
	vec3 ambient = light.ambient * diffuseSample;

	//Return sum
	return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight pointLight, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	float lightDistance = length(lightPos - vert_worldPos);
	float attenuation = 1.0 / (pointLight.attenConstant + pointLight.attenLinear * lightDistance + 
				pointLight.attenQuadratic * (lightDistance * lightDistance));

	//Calculate diffuse
	vec3 lightDir = normalize(lightPos - vert_worldPos);
	float litAmount = max(dot(normal, lightDir), 0);
	vec3 diffuseSample = texture(material.diffuseMap, vert_uv).xyz;
	vec3 diffuse = litAmount * pointLight.diffuse * diffuseSample * attenuation;

	//Calculate ambient
	vec3 ambient = pointLight.ambient * attenuation * diffuseSample;

	//Calculate specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float specAmount = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specularSample = texture(material.specularMap, vert_uv).xyz;
	vec3 specular = specAmount * pointLight.specular  * attenuation * specularSample;

	//Return sum
	return ambient + diffuse + specular;
}

void main()
{
	vec3 normal = normalize(vert_normal);
	vec3 viewDir = normalize(viewPos - vert_worldPos);
	

	vec3 lighting = CalcDirLight(dirLight, normal, viewDir);

	for(int i = 0; i < NUM_POINT_LIGHTS; i++)
	{
		lighting += CalcPointLight(pointLights[i], normal, vert_worldPos, viewDir);
	}
	
	frag_color = vec4(lighting, 1.0);
}
