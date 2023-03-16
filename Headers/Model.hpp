#pragma once

#include "Texture.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"

class Model
{
	// Attrbites
public:
	Texture texture;
	Mesh mesh;
	Shader shader;

// HDR
	Mesh hdrMesh;
	Shader hdrShader;


	// Methods
public:
	glm::mat4 model;
	glm::vec3 position;
	Model();
	Model(Texture& p_texture, Mesh& p_mesh, Shader& p_shader, Mesh& p_hdrMesh, Shader& p_hdrShader);


	void InitTransform(glm::vec3 pos, glm::vec3 scale);
	void Draw();
	void DrawHDR(unsigned int colorBuffer, bool hdr, float exposure);

	void Rotate(float angle, glm::vec3 rotation);
};