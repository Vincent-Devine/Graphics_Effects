#version 330 core
out vec4 FragColor;

flat in vec3 LightingColor; 
varying vec3 testNormal;

//uniform vec3 objectColor;
//
void main()
{
	
  FragColor = vec4(LightingColor , 1.0);
}
