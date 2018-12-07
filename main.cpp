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
#include "glm/gtx/euler_angles.hpp"

const int WINDOW_WIDTH = 1500;
const int WINDOW_HEIGHT = 1200;


#include "ParticleSystem.h"
#include "simpleBox.h"
#include "accumulatedSnow.h"
#include "simpleQuad.h"
#include "common.hpp"

using namespace std;

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

struct Camera {
    glm::vec3 position = glm::vec3(0, 0, 2.9f);
    glm::vec3 up = glm::vec3(0, 1, 0);
    glm::vec3 forward = glm::vec3(0, 0, -1.0f);
    float rotAboutUpAxis = 0;
    float rotAboutLeftAxis = 0;
    float moveXAxis = 0;
    float moveZAxis = 0;
};

Camera camera;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    glm::vec3 left = glm::normalize(glm::cross(camera.up, camera.forward));

    if (key == GLFW_KEY_W) {
        camera.moveZAxis = -1;
        if (action == GLFW_RELEASE) {
            camera.moveZAxis = 0;
        }
    }
    if (key == GLFW_KEY_S) {
        camera.moveZAxis = 1;
        if (action == GLFW_RELEASE) {
            camera.moveZAxis = 0;
        }
    }
    if (key == GLFW_KEY_A) {
        camera.moveXAxis = -1;
        if (action == GLFW_RELEASE) {
            camera.moveXAxis = 0;
        }
    }
    if (key == GLFW_KEY_D) {
        camera.moveXAxis = 1;
        if (action == GLFW_RELEASE) {
            camera.moveXAxis = 0;
        }
    }
    if (key == GLFW_KEY_LEFT) {
        if (action == GLFW_PRESS) {
            camera.rotAboutUpAxis = 1.0f;
        } else if (action == GLFW_RELEASE) {
            camera.rotAboutUpAxis = 0.0f;
        }
    }
    if (key == GLFW_KEY_RIGHT) {
        if (action == GLFW_PRESS) {
            camera.rotAboutUpAxis = -1.0f;
        } else if (action == GLFW_RELEASE) {
            camera.rotAboutUpAxis = 0.0f;
        }
    }
    if (key == GLFW_KEY_UP) {
        if (action == GLFW_PRESS) {
            camera.rotAboutLeftAxis = 1.0f;
        } else if (action == GLFW_RELEASE) {
            camera.rotAboutLeftAxis = 0.0f;
        }
    }
    if (key == GLFW_KEY_DOWN) {
        if (action == GLFW_PRESS) {
            camera.rotAboutLeftAxis = -1.0f;
        } else if (action == GLFW_RELEASE) {
            camera.rotAboutLeftAxis = 0.0f;
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
    GLuint snowProgram = createShaderProgram("shaders\\snowVertex.glsl", "shaders\\snowFragment.glsl");
    GLuint phongShader = createShaderProgram("shaders\\phongVertex.glsl", "shaders\\phongFragment.glsl");
    GLuint shaderProgramDepth = createShaderProgram("shaders\\phongVertex.glsl", "shaders\\fragmentDepth.glsl");
    GLuint particleShader = createShaderProgram("shaders\\vertex.glsl", "shaders\\particleFragment.glsl");
    GLuint shaderProgramInstanced = createShaderProgram("shaders\\instanceVertex.glsl", "shaders\\fragment.glsl");
    GLuint shaderProgramInstancedDepth = createShaderProgram("shaders\\instanceVertex.glsl", "shaders\\fragmentDepth.glsl");
    
    ParticleSystem ps{100000};
    ps.scale = glm::vec3(0.009f, 0.009f, 1.0f);
    ps.colorTexture = loadPNGTexture("images/snow2.png");
    ParticleSystem ps2{100000};
    ps2.scale = glm::vec3(0.01f, 0.01f, 1.0f);
    ps2.colorTexture = loadPNGTexture("images/snow2.png");

    glfwSetKeyCallback(window, keyCallback);

    glm::mat4 perspective = glm::perspectiveFov(1.0f, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT, 0.01f, 1000.0f);
    glm::mat4 depthCameraMatrix = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0,-1.0f,0), glm::vec3(0,0,-1));
    glm::mat4 orthoProjection = glm::ortho(-(float)WINDOW_WIDTH/2.0f,(float)WINDOW_WIDTH/2.0f,-(float)WINDOW_HEIGHT/2.0f,(float)WINDOW_HEIGHT/2.0f,-20.0f,20.0f);
    glm::mat4 orthoProjectionDepth = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,-10.0f,10.0f);
    
    glEnable(GL_DEPTH_TEST);

    SimpleBox simpleBox{};
    SimpleBox ground{};
    SimpleBox simpleBox3{};

    simpleBox.position.y = -2.0f;
    simpleBox.position.z = -0.3f;
    simpleBox.position.x = -0.3f;
    simpleBox.scale = glm::vec3(0.4f, 0.2f, 0.2f);
    ground.position.z = 0.0f;
    ground.position.y = -3.5f;
    ground.position.x = 0.0f;
    ground.scale = glm::vec3(5.0f, 0.2f, 5.0f);
    ground.rotation = glm::eulerAngleZ(0.04f);
    ground.textureScale = 1;

    simpleBox3.position = glm::vec3(-0.0f, 0.5f, 0.0f);
    simpleBox3.scale = glm::vec3(0.8f, 0.04f, 0.8f);
    simpleBox3.rotation = glm::eulerAngleZ(-3.14/4.0f);

    SimpleBox simpleBox4{};
    simpleBox4.position = glm::vec3(0.8f, -1.0f, 0.0f);
    simpleBox4.scale = glm::vec3(0.8f, 0.04f, 0.8f);
    simpleBox4.rotation = glm::eulerAngleZ(3.14/3.0f);
    simpleBox4.textureId = loadPNGTexture("images/blue.png");


    simpleBox3.textureId = loadPNGTexture("images/blue.png");
    ground.textureId = loadPNGTexture("images/blue.png");
    GLuint whiteTexture = loadPNGTexture("images/white.png");
    GLuint snowOffset = loadPNGTexture("images/snowOffset.png");
    

    DepthFBO fboDepth = createFBOForDepth();

    // This is the quad in lower right corner used for debugging depth texture
    SimpleQuad simpleQuad{};
    float simpleQuadSize = 300.0f;
    simpleQuad.position = glm::vec3(WINDOW_WIDTH/2.0f - simpleQuadSize/2.0f, -WINDOW_HEIGHT/2.0f + simpleQuadSize/2.0f, 0);
    simpleQuad.scale = glm::vec3(simpleQuadSize, simpleQuadSize, 1.0f);
    simpleQuad.textureId = fboDepth.texture;
    

    glm::mat4 identity = glm::mat4(1.0f);

    glm::mat4 toLightSpace = orthoProjectionDepth * depthCameraMatrix;

    glfwSwapInterval(0);

    glEnable(GL_TEXTURE_3D);
  

    double previousTimeFPS = glfwGetTime();
    double previousTime = glfwGetTime();
    int frameCounter = 0;
    float dt = 0.16f;

    int iteration = 0;

    SnowMesh snowMesh;

    GLuint ssbo;
    glGenBuffers(1, &ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 1024*1024 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);



    while (!glfwWindowShouldClose(window)) {
        glClearColor(95/255.0f, 111/255.0f, 119/255.0f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Calculate dt and FPS
        double currentTime = glfwGetTime();
        dt = currentTime - previousTime;
        previousTime = currentTime;
        if (currentTime - previousTimeFPS > 1.0f) {
            int fps = frameCounter;
            string title = "FPS: " + to_string(fps) + " - Frame time: " + to_string(dt) + " s";
            glfwSetWindowTitle(window, title.c_str());
            frameCounter = 0;
            previousTimeFPS = currentTime;
        }
        frameCounter++;

        // We build the snow mesh at the second iteration, since at iteration 0
        // the depth buffer has not been written to.
        if (iteration == 1) {
            snowMesh = buildMesh(fboDepth.fbo, fboDepth.texture, glm::vec3());
        }
        
        // Camera movement
        float speed = 1.1f * dt;
        float rotSpeed = 1.1f * dt;
        if (camera.moveXAxis != 0) {
            glm::vec3 right = glm::normalize(glm::cross(camera.forward, camera.up));
            camera.position = camera.position + right * camera.moveXAxis * speed;
        }
        if (camera.moveZAxis != 0) {
            camera.position = camera.position - camera.forward * camera.moveZAxis * speed;
        }
        if (camera.rotAboutUpAxis > 0.1f || camera.rotAboutUpAxis < 0.1f) {
            camera.forward = glm::normalize(glm::rotate(camera.forward, rotSpeed * camera.rotAboutUpAxis, glm::vec3(0, 1, 0)));   
            camera.up = glm::normalize(glm::rotate(camera.up, rotSpeed * camera.rotAboutUpAxis, glm::vec3(0, 1, 0)));   
        }
        if (camera.rotAboutLeftAxis > 0.1f || camera.rotAboutLeftAxis < 0.1f) {
            glm::vec3 left = glm::normalize(glm::cross(camera.up, camera.forward));
            camera.up = glm::normalize(glm::rotate(camera.up, rotSpeed * camera.rotAboutLeftAxis, left));
            camera.forward = glm::normalize(glm::cross(left, camera.up));
        }
        glm::vec3 lookAtCenter = camera.position + camera.forward * 10.0f;
        glm::mat4 cameraMatrix = glm::lookAt(camera.position, lookAtCenter, camera.up);
        
        // Update particles on GPU
        updateParticlesOnGPU(ps, particleShader, cameraMatrix, perspective, toLightSpace, fboDepth.texture, dt); // updates particle system
        updateParticlesOnGPU(ps2, particleShader, cameraMatrix, perspective, toLightSpace, fboDepth.texture, dt); // updates particle system



        // begin render depth
        glViewport(0, 0, shadow_width, shadow_height);
        glBindFramebuffer(GL_FRAMEBUFFER, fboDepth.fbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ground.render(shaderProgramDepth, orthoProjectionDepth, depthCameraMatrix, toLightSpace, fboDepth.texture);
        simpleBox3.render(shaderProgramDepth, orthoProjectionDepth, depthCameraMatrix, toLightSpace, fboDepth.texture);
        simpleBox4.render(shaderProgramDepth, orthoProjectionDepth, depthCameraMatrix, toLightSpace, fboDepth.texture);
        
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // end render depth





        ground.render(phongShader, perspective, cameraMatrix, toLightSpace, fboDepth.texture);
        simpleBox3.render(phongShader, perspective, cameraMatrix, toLightSpace, fboDepth.texture);
        simpleBox4.render(phongShader, perspective, cameraMatrix, toLightSpace, fboDepth.texture);
        simpleQuad.render(shaderProgram, orthoProjection, identity);

        if (iteration > 1) {
            glEnable(GL_BLEND);
           glBlendFunc (GL_ONE, GL_ONE);
            glDepthMask(GL_FALSE);
            renderSnowMesh(snowMesh, whiteTexture, snowOffset, snowProgram, toLightSpace, perspective, cameraMatrix);
            glDisable(GL_BLEND);
            glDepthMask(GL_TRUE);
        }


        glEnable(GL_BLEND);
        glDepthMask(GL_FALSE);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        renderInstanced(ps, shaderProgramInstanced, cameraMatrix, perspective); // Renders particles
        renderInstanced(ps2, shaderProgramInstanced, cameraMatrix, perspective); // Renders particles
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);


        
        glfwPollEvents();
        glfwSwapBuffers(window);
        iteration++;
    }

    glfwTerminate();
}
