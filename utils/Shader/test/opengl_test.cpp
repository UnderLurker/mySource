#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <stb_image.h>

#include "camera.h"
#include "program.h"
#include "shader.h"
#include "texture.h"
#include "vertex_array_obj.h"
#include "const_vertices.h"
using namespace std;

// settings
const unsigned int SCR_WIDTH  = 800;
const unsigned int SCR_HEIGHT = 600;

myUtil::Camera camera;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void cursorPos_callback(GLFWwindow* window, double xpos, double ypos);
void scrollCallBack(GLFWwindow* window, double xoffset, double yoffset);

float deltaTime = 0;
float lastTime  = 0;

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursorPos_callback);
    glfwSetScrollCallback(window, scrollCallBack);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    myUtil::Program cubeProgram("../../../shader/cubeVertex.vs", myUtil::Shader::VERTEX_SHADER,
                            "../../../shader/cubeFragment.fs", myUtil::Shader::FRAGMENT_SHADER);
    cubeProgram.linkProgram();
    myUtil::Program lightProgram("../../../shader/lightFragment.fs", myUtil::Shader::FRAGMENT_SHADER,
                                 "../../../shader/lightVertex.vs", myUtil::Shader::VERTEX_SHADER);
    lightProgram.linkProgram();

    myUtil::VertexArrayObj cubeVAO;
    cubeVAO.setArrayBuffer(vertices, myUtil::VertexArrayObj::STATIC_DRAW);
    cubeVAO.setVertexAttribPointer<float>(3, myUtil::FLOAT, 5, 0);
    cubeVAO.setVertexAttribPointer<float>(2, myUtil::FLOAT, 5, 3);

    myUtil::VertexArrayObj lightVAO;
    lightVAO.setArrayBuffer(lightVertices, myUtil::VertexArrayObj::STATIC_DRAW);
    lightVAO.setEleArrayBuffer(lightIndices, myUtil::VertexArrayObj::STATIC_DRAW);
    lightVAO.setVertexAttribPointer<float>(3, myUtil::FLOAT, 3, 0);

    // load and create a texture
    // -------------------------
    unsigned int texture1, texture2;
    unsigned char* data = nullptr;
    int width, height, nrChannels;
    //    stbi_set_flip_vertically_on_load(true);
    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    data = stbi_load("../../../img/Image/1.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    // texture 2
    // ---------
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    data = stbi_load("../../../img/Image/7.png", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    cubeProgram.use();
    cubeProgram.setInt("texture1", 0);
    cubeProgram.setInt("texture2", 1);
    //    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glEnable(GL_DEPTH_TEST);
    vector<glm::vec3> cubePositions {glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
                                     glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
                                     glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
                                     glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
                                     glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(1, 1, 1, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto currentTime = (float)glfwGetTime();
        deltaTime        = currentTime - lastTime;
        lastTime         = currentTime;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        cubeProgram.use();

        glm::mat4 view(1.0f);
        glm::mat4 project(1.0f);
        view    = camera.getViewMat4();
        project = glm::perspective(glm::radians(camera.getFov()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        cubeProgram.setMatrix4fv("view", glm::value_ptr(view));
        cubeProgram.setMatrix4fv("project", glm::value_ptr(project));

        cubeVAO.use();
        for (int i = 0; i < cubePositions.size(); i++) {
            glm::mat4 model(1.0f);
            float angle = 20.0f * i;
            model       = glm::translate(model, cubePositions[i]);
            model       = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            cubeProgram.setMatrix4fv("model", glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        //        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        lightProgram.use();
        glm::mat4 lightView(1.0f);
        glm::mat4 lightModel(1.0f);
        lightView = camera.getViewMat4();
        lightModel = glm::translate(lightModel, glm::vec3(1, 1, -1));
        lightModel = glm::scale(lightModel, glm::vec3(0.2, 0.2, 0.2));
        lightProgram.setMatrix4fv("view", glm::value_ptr(lightView));
        lightProgram.setMatrix4fv("project", glm::value_ptr(project));
        lightProgram.setMatrix4fv("model", glm::value_ptr(lightModel));
        lightVAO.use();
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    cubeVAO.deleteVAO();
    lightVAO.deleteVAO();
    lightProgram.deleteProgram();
    cubeProgram.deleteProgram();

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window) {
    float speed        = deltaTime * 2.5f;
    uint32_t direction = 0;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) direction |= myUtil::FRONT;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) direction |= myUtil::BACK;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) direction |= myUtil::LEFT;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) direction |= myUtil::RIGHT;
    camera.setCameraSpeed(speed);
    camera.moveProcess(direction);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); }

void cursorPos_callback(GLFWwindow* window, double xpos, double ypos) {
    camera.cursorProcess((float)xpos, (float)ypos);
}

void scrollCallBack(GLFWwindow* window, double xoffset, double yoffset) {
    camera.scrollProcess((float)xoffset, (float)yoffset);
}
