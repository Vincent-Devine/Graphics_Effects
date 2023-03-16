#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_glfw.h"
#include "ImGUI/imgui_impl_opengl3.h"

#include <vector>
#include "Camera.hpp"

#include "Mesh.hpp"
#include "Shader.hpp"
#include "Light.hpp"
#include "Texture.hpp"
#include "Model.hpp"

class App
{
	// Attributes
private:
	GLFWwindow* window;

	Shader shader;
	Shader hdrShader;
	Shader blinnPhong;
	Shader phong;
	Shader gouraud;
	Shader flat;

	Texture woodTexture;

	unsigned int hdrFBO;
	unsigned int colorBuffer;

	// timing
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	Mesh cube;
	Mesh quad;
	Mesh sphere;

	Model modelHDR;
	Model modelLightsBlinnPhong;
	Model modelLightsPhong;
	Model modelLightsGouraud;
	Model modelLightsFlat;

	std::vector<Light> lightsM;
	// Method
public:
	App();
	~App();

	int Init();
	void Update();

private:
	int InitOpenGL();
	void InitShader();
	void InitTexture();
	void InitFramebuffer();
	void InitMesh();
	void InitModel();

	void UpdateDeltaTime();
	void InitLightsM();
	void InitIMGUI();
	void UpdateIMGUI();
};