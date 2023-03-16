#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

flat out vec3 LightingColor; // resulting color from lighting calculations

struct Light 
{
    vec3 Position;
    vec3 Color;
};
uniform Light lights[16];

uniform vec3 viewPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 viewPosition;

void main()
{

    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
    vec3 Position = vec3(model * vec4(aPos, 1.0));
    vec3 Normal = mat3(transpose(inverse(model))) * aNormal;
    vec3 ambient, diffuse, specular;
    int i = 7;

        // gouraud shading
        // ------------------------
    
        // ambient
        float ambientStrength = 0.1;
        ambient = ambientStrength * lights[i].Color;
  	
        // diffuse 
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lights[i].Position - Position);
        float diff = max(dot(norm, lightDir), 0.0);
        diffuse = diff * lights[i].Color;
    
        // specular
        float specularStrength = 1.0; // this is set higher to better show the effect of Gouraud shading 
        vec3 viewDir = normalize(viewPos - Position);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        specular = specularStrength * spec * lights[i].Color;      

        LightingColor = ambient + diffuse + specular;
}