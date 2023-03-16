#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform bool inverse_normals;

void main()
{
//
//vec3 modelViewVertex = vec3(model *view) * aPos;
//vec3 modelViewNormal = vec3(vec4(model *view) * vec4(a_Normal, 0.0));
// float distance = length(u_LightPos - modelViewVertex);
//vec3 lightVector = normalize(u_LightPos - modelViewVertex);
//
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));   
    vs_out.TexCoords = aTexCoords;
    
    vec3 n = inverse_normals ? -aNormal : aNormal;
    
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vs_out.Normal = normalize(normalMatrix * n);
    
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}


//  vec3 modelViewVertex = vec3(u_MVMatrix * a_Position);
// 
//    // Transform the normal's orientation into eye space.
//    vec3 modelViewNormal = vec3(u_MVMatrix * vec4(a_Normal, 0.0));
// 
//    // Will be used for attenuation.
//    float distance = length(u_LightPos - modelViewVertex);
// 
//    // Get a lighting direction vector from the light to the vertex.
//    vec3 lightVector = normalize(u_LightPos - modelViewVertex);
// 
//    // Calculate the dot product of the light vector and vertex normal. If the normal and light vector are
//    // pointing in the same direction then it will get max illumination.
//    float diffuse = max(dot(modelViewNormal, lightVector), 0.1);
// 
//    // Attenuate the light based on distance.
//    diffuse = diffuse * (1.0 / (1.0 + (0.25 * distance * distance)));
// 
//    // Multiply the color by the illumination level. It will be interpolated across the triangle.
//    v_Color = a_Color * diffuse;
// 
//    // gl_Position is a special variable used to store the final position.
//    // Multiply the vertex by the matrix to get the final point in normalized screen coordinates.
//    gl_Position = u_MVPMatrix * a_Position;