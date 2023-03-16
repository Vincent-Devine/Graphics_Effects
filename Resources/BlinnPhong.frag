#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

struct Light {
    vec3 Position;
    vec3 Color;
};

uniform Light lights[16];
uniform sampler2D diffuseTexture;
uniform vec3 viewPos;

void main()
{           
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    
    // lighting
    vec3 lighting = vec3(0.0);


    // ambient
    vec3 ambient = 0.0 * color;

    int i = 4;
        // diffuse
        vec3 lightDir = normalize(lights[i].Position - fs_in.FragPos);
        vec3 normal = normalize(fs_in.Normal);
        float diff = max(dot(lightDir, normal), 0.0);
        vec3 diffuse = lights[i].Color * diff * color;      
        
        // specular
        vec3 viewDir = normalize(viewPos - fs_in.FragPos);
        vec3 reflectDir = reflect(-lightDir, normal);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), 32);
        vec3 specular = vec3(1.0) * spec;
    
        vec3 result = diffuse + specular;
        float distance = length(fs_in.FragPos - lights[i].Position);
        result *= 1.0 / (distance * distance);
        lighting += result;
    FragColor = vec4(ambient + lighting, 1.0);
}