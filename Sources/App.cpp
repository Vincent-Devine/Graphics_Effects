#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <STB_Image/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_glfw.h"
#include "ImGUI/imgui_impl_opengl3.h"

#include "App.hpp"
#include "Camera.hpp"
#include "Shader.hpp"

#include <iostream>
#include <cmath>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window, float deltaTime);
unsigned int loadTexture(const char* path, bool gammaCorrection);
void renderQuad();
void renderCube();
void renderSphere();

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
bool hdr = true;
bool hdrKeyPressed = false;
float exposure = 10.f;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse;

App::App()
    : window(nullptr)
{
}

App::~App()
{
    glfwTerminate();
}

int App::Init()
{
    if (InitOpenGL() != 0)
        return -1;

    InitLightsM();
    InitShader();
    InitTexture();
    InitFramebuffer();
    InitMesh();

    InitModel();
    
    InitIMGUI();

    modelHDR.InitTransform({ 0.f,0.f, 45.f }, { 5.f, 5.f, 25.f });
    modelLightsBlinnPhong.InitTransform({ 7.f, 0.f, -5.6f }, { 1.f, 1.f, 1.f });
    modelLightsPhong.InitTransform({ 3.2f, 0.f, -5.6f }, { 1.f, 1.f, 1.f });
    modelLightsGouraud.InitTransform({ -0.6f, 0.f, -5.6f }, {1.f, 1.f, 1.f});
    modelLightsFlat.InitTransform({ -4.4f, 0.f, -5.6f }, {1.f, 1.f, 1.f});

    return 0;
}

void App::Update()
{
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        UpdateDeltaTime();

        // input
        // -----
        processInput(window, deltaTime);
        
        // update
        // ------
        modelLightsBlinnPhong.Rotate((0.3 * 2 * 3.14) / 360, glm::vec3(0, 1, 0));
        modelLightsPhong.Rotate((0.3 * 2 * 3.14) / 360, glm::vec3(0, 1, 0));
        modelLightsGouraud.Rotate((0.3 * 2 * 3.14) / 360, glm::vec3(0, 1, 0));
        modelLightsFlat.Rotate((0.3 * 2 * 3.14) / 360, glm::vec3(0, 1, 0));

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 1. render scene into floating point framebuffer
        // -----------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        


        camera.Draw(blinnPhong, (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT);
        modelLightsBlinnPhong.Draw();
        // set lighting uniforms
        for (unsigned int i = 0; i < lightsM.size(); i++)
        {
            blinnPhong.SetVec3("lights[" + std::to_string(i) + "].Position", lightsM[i].position);
            blinnPhong.SetVec3("lights[" + std::to_string(i) + "].Color", lightsM[i].color);
        }

        camera.Draw(phong, (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT);
        modelLightsPhong.Draw();
        // set lighting uniforms
        for (unsigned int i = 0; i < lightsM.size(); i++)
        {
            phong.SetVec3("lights[" + std::to_string(i) + "].Position", lightsM[i].position);
            phong.SetVec3("lights[" + std::to_string(i) + "].Color", lightsM[i].color);
        }

        camera.Draw(shader, (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT);
        modelHDR.Draw();
        for (unsigned int i = 0; i < lightsM.size(); i++)
        {
            shader.SetVec3("lights[" + std::to_string(i) + "].Position", lightsM[i].position);
            shader.SetVec3("lights[" + std::to_string(i) + "].Color", lightsM[i].color);
        }

        camera.Draw(gouraud, (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT);
        modelLightsGouraud.Draw();
        for (unsigned int i = 0; i < lightsM.size(); i++)
        {
            gouraud.SetVec3("lights[" + std::to_string(i) + "].Position", lightsM[i].position);
            gouraud.SetVec3("lights[" + std::to_string(i) + "].Color", lightsM[i].color);
        }


        camera.Draw(flat, (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT);
        modelLightsFlat.Draw();
        for (unsigned int i = 0; i < lightsM.size(); i++)
        {
            flat.SetVec3("lights[" + std::to_string(i) + "].Position", lightsM[i].position);
            flat.SetVec3("lights[" + std::to_string(i) + "].Color", lightsM[i].color);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // 2. now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
        // --------------------------------------------------------------------------------------------------------------------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        modelHDR.DrawHDR(colorBuffer, hdr, exposure);

        std::cout << "hdr: " << (hdr ? "on" : "off") << "| exposure: " << exposure << std::endl;

        UpdateIMGUI();
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

    double x, y;
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwGetCursorPos(window, &x, &y);
        mouse_callback(window, x, y);
    }
    else
    {
        firstMouse = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !hdrKeyPressed)
    {
        hdr = !hdr;
        hdrKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
    {
        hdrKeyPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        if (exposure > 0.0f)
            exposure -= 0.001f;
        else
            exposure = 0.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        exposure += 0.001f;
    }
}

int App::InitOpenGL()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    return 0;
}

void App::InitShader()
{
    // build and compile shaders
    // -------------------------
    shader = Shader("Resources/VertexShaderSource.vert", "Resources/FragmentShaderSource.frag");
    hdrShader = Shader("Resources/hdr.vert", "Resources/hdr.frag");
    blinnPhong = Shader("Resources/BlinnPhong.vert", "Resources/BlinnPhong.frag");
    phong = Shader("Resources/Phong.vert", "Resources/Phong.frag");
    gouraud = Shader("Resources/GShader.vert", "Resources/GShader.frag");
    flat = Shader("Resources/FlatShader.vert", "Resources/FlatShader.frag");

    // shader configuration
    // --------------------
    shader.Use();
    shader.SetInt("diffuseTexture", 0);
    hdrShader.Use();
    hdrShader.SetInt("hdrBuffer", 0);
    blinnPhong.Use();
    blinnPhong.SetInt("diffuseTexture", 0);
    phong.Use();
    phong.SetInt("diffuseTexture", 0);
    gouraud.Use();
    gouraud.SetVec3("objectColor", glm::vec3(0.1f, 0.1f, 0.1f));
}

void App::InitTexture()
{
    // load textures
    // -------------
    woodTexture = Texture("resources/wood.jpg", true); // note that we're loading the texture as an SRGB texture
}

void App::InitFramebuffer()
{
    // configure floating point framebuffer
    // ------------------------------------
    glGenFramebuffers(1, &hdrFBO);
    // create floating point color buffer
    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // create depth buffer (renderbuffer)
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    // attach buffers
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void App::InitMesh()
{
    cube.InitCube();
    quad.InitQuad();
    sphere.InitSphere();
}

void App::InitModel()
{
    modelHDR = Model(
        woodTexture,
        cube,
        shader,
        quad,
        hdrShader
    );

    modelLightsBlinnPhong = Model(
        woodTexture,
        sphere,
        blinnPhong,
        quad,
        hdrShader
    );

    blinnPhong.Use();
    modelLightsBlinnPhong.mesh.InitSphere();
    modelLightsBlinnPhong.texture = Texture("resources/wood.jpg", true);
    modelLightsBlinnPhong.hdrMesh.InitQuad();

    modelLightsPhong = Model(
        woodTexture,
        sphere,
        phong,
        quad,
        hdrShader
    );

    phong.Use();
    modelLightsPhong.mesh.InitSphere();
    modelLightsPhong.texture = Texture("resources/wood.jpg", true);
    modelLightsPhong.hdrMesh.InitQuad();

    modelLightsGouraud = Model(
        woodTexture,
        sphere,
        gouraud,
        quad,
        hdrShader
    );

    gouraud.Use();
    modelLightsGouraud.mesh.InitSphere();
    modelLightsGouraud.texture = Texture("resources/wood.jpg", true);
    modelLightsGouraud.hdrMesh.InitQuad();

    modelLightsFlat = Model(
        woodTexture,
        sphere,
        flat,
        quad,
        hdrShader
    );

    flat.Use();
    modelLightsFlat.mesh.InitSphere();
    modelLightsFlat.texture = Texture("resources/wood.jpg", true);
    modelLightsFlat.hdrMesh.InitQuad();
}

void App::UpdateDeltaTime()
{
    // per-frame time logic
    // --------------------
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{

    float xpos = xposIn;
    float ypos = yposIn;
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path, bool gammaCorrection)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum internalFormat;
        GLenum dataFormat;
        if (nrComponents == 1)
        {
            internalFormat = dataFormat = GL_RED;
        }
        else if (nrComponents == 3)
        {
            internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
            dataFormat = GL_RGB;
        }
        else if (nrComponents == 4)
        {
            internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
            dataFormat = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void App::InitLightsM()
{

    modelHDR.InitTransform({ 0.f,0.f, 45.f }, { 5.f, 5.f, 25.f });
    modelLightsBlinnPhong.InitTransform({ 7.f, 0.f, -5.6f }, { 1.f, 1.f, 1.f });
    modelLightsPhong.InitTransform({ 3.2f, 0.f, -5.6f }, { 1.f, 1.f, 1.f });
    modelLightsGouraud.InitTransform({ -0.6f, 0.f, -5.6f }, { 1.f, 1.f, 1.f });
    modelLightsFlat.InitTransform({ -4.4f, 0.f, -5.6f }, { 1.f, 1.f, 1.f });
    // lighting info
// -------------
// lights cubes
    lightsM.push_back(Light(glm::vec3(-4.3f, 4.3f, 68.1f), glm::vec3(500.0f, 500.0f, 500.0f))); // back light
    lightsM.push_back(Light(glm::vec3(-4.3f, -4.3f, 27.5f), glm::vec3(1.f, 0.0f, 0.0f)));
    lightsM.push_back(Light(glm::vec3(4.3f, -4.3f, 33.3f), glm::vec3(0.0f, 1.0f, 1.0f)));
    lightsM.push_back(Light(glm::vec3(4.3f, -4.3f, 47.8f), glm::vec3(0.0f, 1.0f, 0.0f)));

// lights spheres
    lightsM.push_back(Light(glm::vec3( 7.2f, 1.4f, -1.4f ), glm::vec3(10.f, 10.f, 10.f)));
    lightsM.push_back(Light(glm::vec3(4.3f, 1.4f, -1.4f), glm::vec3(10.f, 10.f, 10.f)));
    lightsM.push_back(Light(glm::vec3( 15.f, 15.9f, 3.f ), glm::vec3(0.f, 1.f, 1.f)));
    lightsM.push_back(Light(glm::vec3(73.9f, 42.f, 3.f), glm::vec3(0.5f, 0.f, 0.f)));
}

void App::InitIMGUI()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void App::UpdateIMGUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Lights");
        ImGui::SliderFloat("Exposure ",&exposure, 0.f, 20.f, "%.1f", 0);
        if(ImGui::Button("HDR", { 50.f, 25.f }))
            hdr = !hdr;
    for (unsigned int i = 0; i < lightsM.size(); i++)
    {
        if (ImGui::CollapsingHeader(("Lights " + std::to_string(i)).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::SliderFloat3(("Position" + std::to_string(i)).c_str(),
                &lightsM[i].position.x, -100.f, 100.f, "%.1f", 0);
            ImGui::ColorEdit3(("Color" + std::to_string(i)).c_str(), &lightsM[i].color.x);
        }
    }
    ImGui::End();


    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

