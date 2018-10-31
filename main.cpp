#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#include "glew.h"
#include "glfw3.h"

#define GLM_ENABLE_EXPERIMENTAL 
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp" 
#include "glm/vec3.hpp"
#include "glm/gtx/rotate_vector.hpp"

#include "box.h"

using namespace std;
const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 800;
const string title = "Fun with GPU Particles";

string fileContentToString(string pathToFile) {
    std::ifstream inputFile;
    inputFile.open(pathToFile);
    if (!inputFile) {
        cerr << "Could not open file " << pathToFile << endl;
        exit(1);
    }
    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    return buffer.str();
}

GLuint createShaderProgram(string vertexSourcePath, string fragmentSourcePath) {
    string vertexSource = fileContentToString(vertexSourcePath);
    string fragmentSource = fileContentToString(fragmentSourcePath);
    const char* vertexSourceCStr = vertexSource.c_str();
    const char* fragmentSourceCStr = fragmentSource.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSourceCStr, NULL);
    glCompileShader(vertexShader);
    // Error check
    GLint isCompiled = 0;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);
        for (auto const& value : infoLog) {
            cout << value;
        }
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSourceCStr, NULL);
    glCompileShader(fragmentShader);
    // Error check
    isCompiled = 0;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);
        for (auto const& value : infoLog) {
            cout << value;
        }
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // Error check
    GLint isLinked = 0;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);
        vector<GLchar> infoLog(maxLength);
	    glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, &infoLog[0]);
        for (auto const& value : infoLog) {
            cout << value;
        }
    }

    return shaderProgram;
}

glm::vec3 cameraPosition = glm::vec3(0, 0, 0.9f);
glm::vec3 cameraUp = glm::vec3(0, 1, 0);
glm::vec3 cameraForward = glm::vec3(0, 0, -1.0f);
glm::vec3 moveDirection = glm::vec3(0, 0, 0);
float rotAboutUpAxis = 0;
float rotAboutLeftAxis = 0;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    glm::vec3 left = glm::normalize(glm::cross(cameraUp, cameraForward));

  

    if (key == GLFW_KEY_W) {
        moveDirection = cameraForward;
        if (action == GLFW_RELEASE) {
            moveDirection = glm::vec3(0, 0, 0);
        }
    }
    if (key == GLFW_KEY_S) {
        moveDirection = cameraForward  * -1.0f;
        if (action == GLFW_RELEASE) {
            moveDirection = glm::vec3(0, 0, 0);
        }
    }
    if (key == GLFW_KEY_A) {
        moveDirection = left ;
        if (action == GLFW_RELEASE) {
            moveDirection = glm::vec3(0, 0, 0);
        }
    }
    if (key == GLFW_KEY_D) {
        moveDirection = left  * -1.0f;
        if (action == GLFW_RELEASE) {
            moveDirection = glm::vec3(0, 0, 0);
        }
    }
    if (key == GLFW_KEY_LEFT) {
        if (action == GLFW_PRESS) {
            rotAboutUpAxis = 1.0f;
        } else if (action == GLFW_RELEASE) {
            rotAboutUpAxis = 0.0f;
        }
    }
    if (key == GLFW_KEY_RIGHT) {
        if (action == GLFW_PRESS) {
            rotAboutUpAxis = -1.0f;
        } else if (action == GLFW_RELEASE) {
            rotAboutUpAxis = 0.0f;
        }
    }
    if (key == GLFW_KEY_UP) {
        if (action == GLFW_PRESS) {
            rotAboutLeftAxis = 1.0f;
        } else if (action == GLFW_RELEASE) {
            rotAboutLeftAxis = 0.0f;
        }
    }
    if (key == GLFW_KEY_DOWN) {
        if (action == GLFW_PRESS) {
            rotAboutLeftAxis = -1.0f;
        } else if (action == GLFW_RELEASE) {
            rotAboutLeftAxis = 0.0f;
        }
    }
}

int main() {
    if (!glfwInit()) {
        cerr << "Failed glfwInit()" << endl;
    }

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, title.c_str(), NULL, NULL);
    if (!window) {
        cerr << "Failed to create window" << endl;
    }

    glfwMakeContextCurrent(window);

    glewInit();

    GLuint shaderProgram = createShaderProgram("shaders\\vertex.glsl", "shaders\\fragment.glsl");
    Box b{};
    
    glfwSetKeyCallback(window, keyCallback);

    glm::mat4 perspective = glm::perspectiveFov(1.0f, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT, 0.01f, 1000.0f);
    
    float t = 0;
    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        float speed = 0.001f;
        float rotSpeed = 0.001f;
        cameraPosition = cameraPosition + moveDirection * speed;
        if (rotAboutUpAxis > 0.1f || rotAboutUpAxis < 0.1f) {
            cameraForward = glm::normalize(glm::rotate(cameraForward, rotSpeed * rotAboutUpAxis, glm::vec3(0, 1, 0)));   
            cameraUp = glm::normalize(glm::rotate(cameraUp, rotSpeed * rotAboutUpAxis, glm::vec3(0, 1, 0)));   
        }
        if (rotAboutLeftAxis > 0.1f || rotAboutLeftAxis < 0.1f) {
            glm::vec3 left = glm::normalize(glm::cross(cameraUp, cameraForward));
            cameraUp = glm::normalize(glm::rotate(cameraUp, rotSpeed * rotAboutLeftAxis, left));
            cameraForward = glm::normalize(glm::cross(left, cameraUp));
        }

        glm::vec3 lookAtCenter = cameraPosition + cameraForward * 1.0f;
        glm::mat4 camera = glm::lookAt(cameraPosition, lookAtCenter, cameraUp);
        renderBox(b, shaderProgram, camera, perspective);

        t += 0.001f;

        //b.position.x = sin(t); 
        b.scale.x = 1;

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
}
