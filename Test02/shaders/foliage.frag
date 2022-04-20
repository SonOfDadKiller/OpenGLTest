#version 330 core

//Input from Vert Shader
in vec2 vert_uv;
in vec3 vert_normal;
in vec3 vert_worldPos;

//Uniforms
uniform vec3 viewPos;
uniform float near;
uniform float far;

struct Material
{
	sampler2D texture_diffuse0;
	sampler2D texture_specular0;
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

float LinearizeDepth(float depth)
{
	float nonLinearDepth = depth * 2.0 - 1.0;
	return (2.0 * near * far) / (far + near - nonLinearDepth * (far - near));
}

void main()
{
	//Sample diffuse and determine if frag can be discarded
	vec4 diffuseSample = texture(material.texture_diffuse0, vert_uv);

	if (diffuseSample.a < 0.1)
	{
		discard;
	}

	//Calculate light attenuation
	float lightDistance = length(light.position - vert_worldPos);
	float attenuation = 1.0 / (light.attenConstant + light.attenLinear * lightDistance + 
				light.attenQuadratic * (lightDistance * lightDistance));
	

	vec3 viewDir = normalize(viewPos - vert_worldPos);

	//Flip normal if we are drawing a backface
	vec3 normal = normalize(vert_normal);
	if (dot(viewDir, normal) < 0)
	{
		normal = -normal;
	}

	//Calculate diffuse
	vec3 lightDir = normalize(light.position - vert_worldPos);
	float litAmount = max(dot(normal, lightDir), 0);
	

	vec3 diffuse = litAmount * light.diffuse * diffuseSample.xyz * attenuation;

	//Calculate ambient
	vec3 ambient = light.ambient * attenuation * diffuseSample.xyz;

	//Calculate specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float specAmount = pow(max(dot(viewDir, reflectDir), 0.0), 16);
	vec3 specularSample = texture(material.texture_specular0, vert_uv).xyz;
	vec3 specular = specAmount * light.specular  * attenuation * specularSample;

	vec3 lighting = ambient + diffuse + specular;


	//frag_color = vec4(diffuseSample, 1.0);
	
	//frag_color = vec4(lighting, 1.0);


	frag_color = mix(vec4(lighting, diffuseSample.w), vec4(vec3(1), diffuseSample.w), LinearizeDepth(gl_FragCoord.z) / 25);

	//frag_color = vec4(LinearizeDepth(gl_FragCoord.z) / far, 0, 0, 1);

	//frag_color = vec4(vec3(gl_FragCoord.z), 1.0);
}
