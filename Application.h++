#pragma once

#include <iostream>
#include <stdexcept>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"



#include "ShaderProgram.h++"
#include "Camera.h++"


class Application
{
private:
    struct WindowContext
    {
        GLFWwindow* window{ nullptr };
        WindowContext(int width, int height, const char* title);
        ~WindowContext();
    };

    // [IMPORTANT] WindowContext MUST be declared FIRST so it constructs first and destructs last for resolving the "Segmentation fault" error in linux
    WindowContext m_Context;

    // Plain member objects (constructed second, destructed before m_Context)
    ShaderProgram CubeProgram;
    ShaderProgram LightProgram;

    unsigned int VAO{ 0 };
    unsigned int VBO{ 0 };
    unsigned int EBO{ 0 };
    unsigned int LightVAO{ 0 };
    unsigned int texture1{ 0 };
    unsigned int texture2{ 0 };

    Camera m_Camera;
    glm::vec3 m_LightPos{ 1.2f, 1.0f, 2.0f };
    float m_MixValue{ 0.2f };
    float m_LastX{ WINDOW_DEFAULT_WIDTH / 2.0f };
    float m_LastY{ WINDOW_DEFAULT_HEIGHT / 2.0f };
    bool m_FirstMouse{ true };


public:
    Application();
    ~Application();
    void run();

private:
    void setupBuffers();
    void setupTextures();
    void setupCallbacks();
    void onMouseMove(float xpos, float ypos);
    void processInput();
};