#include <iostream>
#include <stdexcept>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h++"

#include "ShaderProgram.h++"
#include "Camera.h++"

const int WINDOW_DEFAULT_HEIGHT { 600 };
const int WINDOW_DEFAULT_WIDTH  { 800 };

extern float vertices[216];
extern float teextures[72];
unsigned int indices[]
{  
    // note that we start from 0!
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
};


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

class Application
{
private:
    struct WindowContext 
    {
        GLFWwindow* window{ nullptr };

        WindowContext(int width, int height, const char* title) 
        {
            if (!glfwInit())
            {
                throw std::runtime_error("Failed to initialize GLFW");
            }

            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

            window = glfwCreateWindow(width, height, title, nullptr, nullptr);
            if (!window) 
            {
                glfwTerminate();
                throw std::runtime_error("Failed to create GLFW window");
            }

            glfwMakeContextCurrent(window);

            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
            {
                glfwDestroyWindow(window);
                glfwTerminate();
                throw std::runtime_error("Failed to initialize GLAD");
            }
        }

        ~WindowContext() 
        {
            if (window) glfwDestroyWindow(window);
            glfwTerminate();
        }
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
    Application()
        : m_Context(WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT, "LearningOpenGL"),
          CubeProgram("VertexShader.vs", "FragmentShader.fs"),
          LightProgram("LightVertexShader.vs", "LightFragmentShader.fs"),
          m_Camera(glm::vec3(0.0f, 0.0f, 6.0f), glm::vec3(0.0f, 1.0f, 0.0f))
    {
        setupCallbacks();
        setupBuffers();
        setupTextures();
    }

    ~Application() 
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteVertexArrays(1, &LightVAO);
        glDeleteBuffers(1, &VBO);
        glDeleteTextures(1, &texture1);
        glDeleteTextures(1, &texture2);
    }

    void run()
    {
        glm::vec3 cubePositions[] = 
        {
            glm::vec3(0.0f,  0.0f,  0.0f),
            glm::vec3(2.0f,  5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3(2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f,  3.0f, -7.5f),
            glm::vec3(1.3f, -2.0f, -2.5f),
            glm::vec3(1.5f,  2.0f, -2.5f),
            glm::vec3(1.5f,  0.2f, -1.5f),
            glm::vec3(-1.3f,  1.0f, -1.5f)
        };
        

        glEnable(GL_DEPTH_TEST);
        glfwSetInputMode(m_Context.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        

        while (!glfwWindowShouldClose(m_Context.window))
        {
            processInput();

            float timeValue = static_cast<float>(glfwGetTime());
            float greenValue = sin(timeValue) / 2.0f;
            glClearColor(0.2f, greenValue, -greenValue, 1.0f); // 0.3f, 0.3f

            glEnable(GL_DEPTH_TEST);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glfwSetInputMode(m_Context.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            m_Camera.deltaUpdate();


            CubeProgram.Use();
            glm::vec3 CubeColor { glm::vec3(1.0f, 0.9f, 0.1f) };
            glm::vec3 LightColor { glm::vec3(1.0f, 1.0f, 1.0f) };
            CubeProgram.setVec3(CubeProgram.ID, "CubeColor", CubeColor);
            CubeProgram.setVec3(CubeProgram.ID, "lightColor", LightColor);
            CubeProgram.setVec3(CubeProgram.ID, "lightPos", m_LightPos);
            CubeProgram.setVec3(CubeProgram.ID, "viewPos", m_Camera.m_Position);
            // FragmentShader.setFloat(Program.ID, "T_Percent", mixValue);
            glm::vec3 ambient   { (1.0f, 0.5f, 0.31f) };
            glm::vec3 diffuse   { (1.0f, 0.5f, 0.31f) };
            glm::vec3 specular  { (0.5f, 0.5f, 0.50f) };
            float shininess     { 32.0f };
            CubeProgram.setVec3 (CubeProgram.ID, "material.ambient", ambient);
            CubeProgram.setVec3 (CubeProgram.ID, "material.diffuse", diffuse);
            CubeProgram.setVec3 (CubeProgram.ID, "material.specular", specular);
            CubeProgram.setFloat(CubeProgram.ID, "material.shininess", shininess);


            // LightColor.x = static_cast<float>(sin(glfwGetTime() * 2.0f));
            // LightColor.y = static_cast<float>(sin(glfwGetTime() * 0.7f));
            // LightColor.z = static_cast<float>(sin(glfwGetTime() * 1.3f));
            glm::vec3 diffuseColor = LightColor * glm::vec3(0.5f);
            glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
            glm::vec3 light_ambient  { (1.0f, 0.5f, 0.31f) };
            glm::vec3 light_diffuse  { (1.0f, 0.5f, 0.31f) };
            glm::vec3 light_specular { (1.0f, 1.0f, 1.00f) };

            glUniform3fv(glGetUniformLocation(CubeProgram.ID, "light.position"), 1, glm::value_ptr(m_LightPos));
            glUniform3fv(glGetUniformLocation(CubeProgram.ID, "light.ambient"), 1, glm::value_ptr(ambientColor));
            glUniform3fv(glGetUniformLocation(CubeProgram.ID, "light.diffuse"), 1, glm::value_ptr(diffuseColor));
            glUniform3fv(glGetUniformLocation(CubeProgram.ID, "light.specular"), 1, glm::value_ptr(light_specular));
            LightProgram.Use();
            glUniform3fv(glGetUniformLocation(LightProgram.ID, "LightColor"), 1, glm::value_ptr(LightColor));
            // LightFragmentShader.setVec3(LightProgram.ID, "LightColor", LightColor);
            

            CubeProgram.Use();
            glm::mat4 model         = glm::mat4(1.0f);
            glm::mat4 view          = glm::mat4(1.0f);
            glm::mat4 projection    = glm::mat4(1.0f);

            projection = glm::perspective(glm::radians(m_Camera.m_Zoom), (float)WINDOW_DEFAULT_WIDTH / (float)WINDOW_DEFAULT_HEIGHT, 0.1f, 100.0f);
            view = m_Camera.viewMatrix();
            
            CubeProgram.setMat4(CubeProgram.ID, "view", view);
            CubeProgram.setMat4(CubeProgram.ID, "projection", projection);


            // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // For wireframe
            // glActiveTexture(GL_TEXTURE0);
            // glBindTexture(GL_TEXTURE_2D, texture1);
            // glActiveTexture(GL_TEXTURE1);
            // glBindTexture(GL_TEXTURE_2D, texture2);

            glBindVertexArray(VAO);

            model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
            CubeProgram.setMat4(CubeProgram.ID, "model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            /*
            for (unsigned int i = 0; i < 10; i++)
            {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, cubePositions[i]);
                float angle = 20.0f * (i + 1);
                model = i % 3 ? glm::rotate(model, static_cast<float>(glfwGetTime()) * glm::radians(angle * (float)std::pow(-1, i)), glm::vec3(0.5f * (i + 1), 1.0f, (float)i)) \
                                : model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
                VertexShader.setMat4(CubeProgram.ID, "model", model);

                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
            */

            // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


            m_LightPos = glm::vec3(glm::cos(static_cast<float>(glfwGetTime())), m_LightPos.y, glm::sin(static_cast<float>(glfwGetTime())));
            model = glm::mat4(1.0f);
            model = glm::translate(model, m_LightPos);
            model = glm::scale(model, glm::vec3(0.2f));

            LightProgram.Use();
            LightProgram.setMat4(LightProgram.ID, "view", view);
            LightProgram.setMat4(LightProgram.ID, "projection", projection);
            LightProgram.setMat4(LightProgram.ID, "model", model);

            
            

            glBindVertexArray(LightVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);



            glfwSwapBuffers(m_Context.window);
            glfwPollEvents();
        }
    }

private:
    void setupBuffers() 
    {
        glGenBuffers(1, &VBO);
        glGenVertexArrays(1, &VAO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // Light VAO
        glGenVertexArrays(1, &LightVAO);
        glBindVertexArray(LightVAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void setupTextures() 
    {
        int width, height, nrChannels;

        // Texture 1
        glGenTextures(1, &texture1);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        unsigned char* data1 = stbi_load("Textures/wall.jpg", &width, &height, &nrChannels, 0);
        if (data1) 
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(data1);
        }

        // Texture 2
        glGenTextures(1, &texture2);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_set_flip_vertically_on_load(true);
        unsigned char* data2 = stbi_load("Textures/awesomeface.png", &width, &height, &nrChannels, 0);
        if (data2) 
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(data2);
        }
    }

    void setupCallbacks() 
    {
        // Associate this Application instance pointer with the GLFW window
        glfwSetWindowUserPointer(m_Context.window, this);

        glfwSetFramebufferSizeCallback(m_Context.window, [](GLFWwindow*, int width, int height) {
            glViewport(0, 0, width, height);
        });

        glfwSetCursorPosCallback(m_Context.window, [](GLFWwindow* window, double xpos, double ypos) {
            auto* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
            if (app) app->onMouseMove(static_cast<float>(xpos), static_cast<float>(ypos));
        });

        glfwSetScrollCallback(m_Context.window, [](GLFWwindow* window, double, double yoffset) {
            auto* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
            if (app) app->m_Camera.mouseScroll(static_cast<float>(yoffset));
        });
    }

    void onMouseMove(float xpos, float ypos) 
    {
        if (m_FirstMouse) 
        {
            m_LastX = xpos;
            m_LastY = ypos;
            m_FirstMouse = false;
        }

        float xoffset = xpos - m_LastX;
        float yoffset = m_LastY - ypos;
        m_LastX = xpos;
        m_LastY = ypos;

        m_Camera.mouseMovement(xoffset, yoffset, true);
    }

    void processInput() 
    {
        if (glfwGetKey(m_Context.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(m_Context.window, true);

        if (glfwGetKey(m_Context.window, GLFW_KEY_W) == GLFW_PRESS)
            m_Camera.keyboardInput(Camera::Direction::FORWARD);
        if (glfwGetKey(m_Context.window, GLFW_KEY_S) == GLFW_PRESS)
            m_Camera.keyboardInput(Camera::Direction::BACKWARD);
        if (glfwGetKey(m_Context.window, GLFW_KEY_A) == GLFW_PRESS)
            m_Camera.keyboardInput(Camera::Direction::LEFT);
        if (glfwGetKey(m_Context.window, GLFW_KEY_D) == GLFW_PRESS)
            m_Camera.keyboardInput(Camera::Direction::RIGHT);
    }

};

int main()
{
    try 
    {
        Application app;
        app.run();
    } 
    catch (const std::exception& e) 
    {
        std::cerr << "Engine Initialization Failed: " << e.what() << '\n';
        return -1;
    }

    return 0;
}


float vertices[] = 
{
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

float textures[] =
{
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,

    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,

    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,

    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,

    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 0.0f,
    0.0f, 1.0f,

    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 0.0f,
    0.0f, 1.0f
};

//float vertices[] = 
//{
//    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
//     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
//     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
//     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
//    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
//    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
//
//    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
//     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
//     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
//     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
//    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
//    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
//
//    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
//    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
//    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//
//     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
//     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
//     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//
//    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
//     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
//     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
//    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
//    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//
//    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
//     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
//     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
//    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
//};