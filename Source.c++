#include <iostream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h++"


#include "Shader.h++"
#include "ShaderProgram.h++"
#include "Camera.h++"

const int WINDOW_DEFAULT_HEIGHT { 600 };
const int WINDOW_DEFAULT_WIDTH  { 800 };

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

float mixValue  { 0.2f };

float lastX { WINDOW_DEFAULT_WIDTH / 2.0f };
float lastY { WINDOW_DEFAULT_HEIGHT / 2.0f };

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

Camera MainCamera(glm::vec3(0.0f, 0.0f, 6.0f), glm::vec3(0.0f, 1.0f, 0.0f));

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    

    GLFWwindow* window = glfwCreateWindow(WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT, "LearningOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT);


    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);




    extern float vertices[216];
    extern float teextures[72];

    unsigned int indices[] = 
    {  
        // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };



    unsigned int VBO;
    glGenBuffers(1, &VBO);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    unsigned int EBO;
    glGenBuffers(1, &EBO);


    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);




    unsigned int LightVAO;
    glGenVertexArrays(1, &LightVAO);
    glBindVertexArray(LightVAO);
    // we only need to bind to the VBO, the container's VBO's data already contains the data.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // set the vertex attribute 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);




    Shader VertexShader(Shader::ShaderType::VERTEX, "VertexShader.vs");
    Shader FragmentShader (Shader::ShaderType::FRAGMENT, "FragmentShader.fs");
    ShaderProgram CubeProgram(VertexShader, FragmentShader);

    VertexShader.~Shader();
    FragmentShader.~Shader();




    Shader LightVertexShader(Shader::ShaderType::VERTEX, "LightVertexShader.vs");
    Shader LightFragmentShader(Shader::ShaderType::FRAGMENT, "LightFragmentShader.fs");
    ShaderProgram LightProgram(LightVertexShader, LightFragmentShader);

    LightVertexShader.~Shader();
    LightFragmentShader.~Shader();



    unsigned int texture1, texture2;

    
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char* data1 = stbi_load("Textures/wall.jpg", &width, &height, &nrChannels, 0);
    if (data1)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data1);


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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data2);


    // Program.Use();
    // FragmentShader.setFloat(Program.ID, "texture1", 0);
    // FragmentShader.setFloat(Program.ID, "texture2", 1);
    // glUniform1i(glGetUniformLocation(Program.ID, "texture2"), 1);


    // FragmentShader.setVec3(Program.ID, "objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
    // glm::vec3 CubeColor { glm::vec3(1.0f, 0.5f, 0.31f) };
    // glUniform3fv(glGetUniformLocation(Program.ID, "objectColor"), 1, &CubeColor[0]);
    // FragmentShader.setVec3(Program.ID, "lightColor",  glm::vec3(1.0f, 1.0f, 1.00f));
    



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
    


    

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        float timeValue = static_cast<float>(glfwGetTime());
        float greenValue = sin(timeValue) / 2.0f;
        glClearColor(0.2f, greenValue, -greenValue, 1.0f); // 0.3f, 0.3f

        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        MainCamera.deltaUpdate();


        CubeProgram.Use();
        glm::vec3 CubeColor { glm::vec3(1.0f, 0.9f, 0.1f) };
        glm::vec3 LightColor { glm::vec3(1.0f, 1.0f, 1.0f) };
        FragmentShader.setVec3(CubeProgram.ID, "CubeColor", CubeColor);
        FragmentShader.setVec3(CubeProgram.ID, "lightColor", LightColor);
        FragmentShader.setVec3(CubeProgram.ID, "lightPos", lightPos);
		FragmentShader.setVec3(CubeProgram.ID, "viewPos", MainCamera.m_Position);
        // FragmentShader.setFloat(Program.ID, "T_Percent", mixValue);
        glm::vec3 ambient   { (1.0f, 0.5f, 0.31f) };
        glm::vec3 diffuse   { (1.0f, 0.5f, 0.31f) };
        glm::vec3 specular  { (0.5f, 0.5f, 0.5f) };
        float shininess     { 32.0f };
        FragmentShader.setVec3 (CubeProgram.ID, "material.ambient", ambient);
        FragmentShader.setVec3 (CubeProgram.ID, "material.diffuse", diffuse);
        FragmentShader.setVec3 (CubeProgram.ID, "material.specular", specular);
        FragmentShader.setFloat(CubeProgram.ID, "material.shininess", shininess);


        // LightColor.x = static_cast<float>(sin(glfwGetTime() * 2.0f));
        // LightColor.y = static_cast<float>(sin(glfwGetTime() * 0.7f));
        // LightColor.z = static_cast<float>(sin(glfwGetTime() * 1.3f));
        glm::vec3 diffuseColor = LightColor * glm::vec3(0.5f);
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
        glm::vec3 light_ambient{ (1.0f, 0.5f, 0.31f) };
        glm::vec3 light_diffuse{ (1.0f, 0.5f, 0.31f) };
        glm::vec3 light_specular{ (1.0f, 1.0f, 1.0f) };

        glUniform3fv(glGetUniformLocation(CubeProgram.ID, "light.position"), 1, glm::value_ptr(lightPos));
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

        projection = glm::perspective(glm::radians(MainCamera.m_Zoom), (float)WINDOW_DEFAULT_WIDTH / (float)WINDOW_DEFAULT_HEIGHT, 0.1f, 100.0f);
        view = MainCamera.viewMatrix();
        
        VertexShader.setMat4(CubeProgram.ID, "view", view);
        VertexShader.setMat4(CubeProgram.ID, "projection", projection);


        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // For wireframe
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, texture1);
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, texture2);

        glBindVertexArray(VAO);

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        VertexShader.setMat4(CubeProgram.ID, "model", model);
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


        lightPos = glm::vec3(glm::cos(static_cast<float>(glfwGetTime())), lightPos.y, glm::sin(static_cast<float>(glfwGetTime())));
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));

        LightProgram.Use();
        LightVertexShader.setMat4(LightProgram.ID, "view", view);
        LightVertexShader.setMat4(LightProgram.ID, "projection", projection);
        LightVertexShader.setMat4(LightProgram.ID, "model", model);

        
        

        glBindVertexArray(LightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);



        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    // glDeleteBuffers(1, &EBO);


    glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        mixValue += 0.001f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        mixValue -= 0.001f;
    if (mixValue >= 1.0f) mixValue = 1.0f;
    if (mixValue <= 0.0f) mixValue = 0.0f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        MainCamera.keyboardInput(Camera::Direction::FORWARD);// cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        MainCamera.keyboardInput(Camera::Direction::BACKWARD);// cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        MainCamera.keyboardInput(Camera::Direction::LEFT);// cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        MainCamera.keyboardInput(Camera::Direction::RIGHT);// cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    static bool firstMouse{ true };
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

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

    MainCamera.mouseMovement(xoffset, yoffset, true);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    MainCamera.mouseScroll(static_cast<float>(yoffset));
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