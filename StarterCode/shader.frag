#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in float sampleExtraOutput;

// You can output many things. The first vec4 type output determines the color of the fragment


////////////////Taken from the learnopengl.com site///////////////////////
struct Material {
    vec3 diffuse;
    vec3 specular;
	vec3 ambient;
    float shininess;
};

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position; 
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
	vec3 direction;
	vec3 position;
	float cutOff;
	float outerCutOff;
	float quadratic;
	float spotExponent; //might need to be a float
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};


#define NR_POINT_LIGHTS 4

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;


out vec4 color;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;
uniform int type;
uniform int mode;
// Function prototypes

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir); //add more things that we need
///////////////////////end copypasta/////////////////////////////



void main()
{
    // Color everything a hot pink color. An alpha of 1.0f means it is not transparent.
    color = vec4(1.0f, 0.41f, 0.7f, sampleExtraOutput);

	/////copypasta//////
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 result;
	
	if(mode == 0){
		if(type == 1 || type == 0){
			//directional light 
			result = CalcDirLight(dirLight, norm, viewDir);
		}
	
		else if (type == 2 || type == 0){
			result = vec3(0.0, 0.0, 0.0);
			for(int i = 0; i < NR_POINT_LIGHTS; i++)
				result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
		}
		
		else if (type == 3 || type == 0){
			//spotlight
			result = CalcSpotLight(spotLight, norm, FragPos, viewDir);
		}
	}
	else if (mode == 1){
		result = norm;
	}
	color = vec4(result, 1.0);
}


//Calculating the color when using a directional light
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction); //in directional lighting, L = -d
	
	//calculating the diffuse shading
	float diff = max(dot(normal, lightDir), 0.0); 

	//calculating the specular shading
	//what we need: reflection vector (reflectDir), view vector (viewDir), and light vector (lightDir)
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	//calculating the ambient shading, combining all of the shades
	vec3 ambient = light.ambient * material.diffuse;
	vec3 diffuse = light.diffuse * (diff * material.diffuse);
	vec3 specular = light.specular * (spec * material.specular);
	return (ambient + diffuse + specular);
}


//Calculating the color when using a point light
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
	vec3 lightDir = normalize(light.position - fragPos); 

	//calculating the diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);

	//calculating the specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	//attenuation calculations
	float distance = length(light.position - fragPos);
	float attenuation = 1.0f / (0.017f * (distance * distance)); 

	//combine results
	vec3 ambient = light.ambient * material.diffuse;
	vec3 specular = light.specular * (spec * material.specular);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient+diffuse+specular);
}


vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
	
	//Ambient
	vec3 ambient = light.ambient * material.diffuse;
	
	//Diffuse
	vec3 lightDir = normalize(light.position - fragPos); //in point lighting, L = (p-v)/||(p-v)||
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);
	
	//Specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * (spec * material. specular);

	//Spotlight soft
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff; //harden, sharpen edges
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	diffuse *= intensity;
	specular *= intensity;

	//attenuation calculations
	float distance = length(light.position - fragPos);
	float attenuation = 1.0f / (0.017f * (distance * distance)); 

	//combine results


	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient+diffuse+specular);

}
