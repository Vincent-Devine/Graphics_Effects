#include "Model.hpp"

Model::Model()
	: texture()
	, mesh()
	, shader()
	, hdrMesh()
	, hdrShader()
	, model(glm::mat4(1.0f))
{

}

Model::Model(Texture& p_texture, Mesh& p_mesh, Shader& p_shader, Mesh& p_hdrMesh, Shader& p_hdrShader)
	: texture(p_texture)
	, mesh(p_mesh)
	, shader(p_shader)
	, hdrMesh(p_hdrMesh)
	, hdrShader(p_hdrShader)
	, model(glm::mat4(1.0f))
{
}

void Model::InitTransform(glm::vec3 pos, glm::vec3 scale)
{
	model = glm::mat4(1.0f);
	model = glm::translate(model, pos);
	model = glm::scale(model, scale);
}

void Model::Draw()
{
	shader.Use();

	texture.Draw();
	// model
	shader.SetMat4("model", model);
	if(mesh.mt == MeshType::MT_Cube)
		shader.SetInt("inverse_normals", true);
	else
		shader.SetInt("inverse_normals", false);
	
	mesh.Draw();
}

void Model::DrawHDR(unsigned int colorBuffer, bool hdr, float exposure)
{
	hdrShader.Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	hdrShader.SetInt("hdr", hdr);
	hdrShader.SetFloat("exposure", exposure);
	hdrMesh.Draw();
}

void Model::Rotate(float angle, glm::vec3 rotation)
{
	model = glm::rotate(model, angle ,rotation);
}
