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

const int WINDOW_WIDTH = 1800;
const int WINDOW_HEIGHT = 950;
const int depthSize = 2048;

#include "particleSystem.hpp"
#include "accumulatedSnow.hpp"
#include "simpleBox.hpp"
#include "simpleQuad.hpp"
#include "model.hpp"
#include "common.hpp"

using namespace std;

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

void updateCamera(float dt) {
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
}

int main() {
    if (!glfwInit()) {
        cerr << "Failed glfwInit()" << endl;
    }

    string title = "Fun with GPU particles";
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, title.c_str(), NULL, NULL);
    if (!window) {
        cerr << "Failed to create window" << endl;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);
    glfwSwapInterval(0);

    glewInit();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_3D);

    printf("OpenGL version supported by this platform (%s): \n", glGetString(GL_VERSION));

    // Create shader programs
    GLuint shaderProgram = createShaderProgram("shaders\\vertex.glsl", "shaders\\fragment.glsl");
    GLuint lowpassProgram = createShaderProgram("shaders\\vertex.glsl", "shaders\\lowpassFragment.glsl");
    GLuint snowProgram = createShaderProgram("shaders\\snowVertex.glsl", "shaders\\snowFragment.glsl");
    GLuint phongShader = createShaderProgram("shaders\\phongVertex.glsl", "shaders\\phongFragment.glsl");
    GLuint shaderProgramDepth = createShaderProgram("shaders\\phongVertex.glsl", "shaders\\fragmentDepth.glsl");
    GLuint particleShader = createShaderProgram("shaders\\vertex.glsl", "shaders\\simulateParticlesFragment.glsl");
    GLuint shaderProgramInstanced = createShaderProgram("shaders\\instanceVertex.glsl", "shaders\\renderParticlesFragment.glsl");
    GLuint shaderCalcNormals = createShaderProgram("shaders\\vertex.glsl", "shaders\\calcNormalsFragment.glsl");

    glm::mat4 perspective = glm::perspectiveFov(1.0f, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT, 0.01f, 1000.0f);
    glm::mat4 depthCameraMatrix = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0,-1.0f,0), glm::vec3(0,0,-1));
    glm::mat4 orthoProjection = glm::ortho(-(float)WINDOW_WIDTH/2.0f,(float)WINDOW_WIDTH/2.0f,-(float)WINDOW_HEIGHT/2.0f,(float)WINDOW_HEIGHT/2.0f,-20.0f,20.0f);
    glm::mat4 orthoProjectionForDepthTexture = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,-10.0f,10.0f);
    glm::mat4 identity = glm::mat4(1.0f);
    glm::mat4 toDepthTextureSpace = orthoProjectionForDepthTexture * depthCameraMatrix;
    glm::mat4 toLightSpace = orthoProjectionForDepthTexture * depthCameraMatrix;

    // Create particle systems
    ParticleSystem ps{100000};
    ps.scale = glm::vec3(0.013f, 0.013f, 1.0f);
    ps.colorTexture = loadPNGTexture("images/snow1.png");
    ParticleSystem ps2{100000};
    ps2.scale = glm::vec3(0.013f, 0.013f, 1.0f);
    ps2.colorTexture = loadPNGTexture("images/snow2.png");
    ParticleSystem ps3{100000};
    ps3.scale = glm::vec3(0.013f, 0.013f, 1.0f);
    ps3.colorTexture = loadPNGTexture("images/snow3.png");
    ParticleSystem ps4{100000};
    ps4.scale = glm::vec3(0.013f, 0.013f, 1.0f);
    ps4.colorTexture = loadPNGTexture("images/snow4.png");

    // Create terrain
    std::vector<float>* heightValues = pngTextureToFloatArray("images/terrain.png");
    float* heightArray = &(*heightValues)[0];
    int size = sqrt(heightValues->size());
    Model terrainModel = heightmapToModel(heightArray, size, size, 1.0f, 1.0f, 1.0f, 30.0f);
    terrainModel.textureId = loadPNGTexture("images/rock.png");
    terrainModel.position = glm::vec3(-12, -5, -12);
    terrainModel.scale =glm::vec3(0.1f, 0.02f, 0.1f);
    terrainModel.textureScale = 3;

    // Create sphere
    Model sphere = tinyObjLoader("models/sphere.obj");
    sphere.textureId =  loadPNGTexture("images/wood.png");
    sphere.position =  glm::vec3(-4.0f, -4.0f, 0.0f);
    sphere.scale =  glm::vec3(0.02f, 0.02f, 0.02f);
    sphere.textureScale = 4;

    // Create cottage
    Model cottage = tinyObjLoader("models/cottage.obj");
    cottage.textureId =  loadPNGTexture("images/wood.png");
    cottage.position =  glm::vec3(-3.0f, -4.96f, -2.0f);
    cottage.scale =  glm::vec3(0.02f, 0.02f, 0.02f);
    cottage.rotation = glm::eulerAngleY(3.14162f);
    cottage.textureScale = 15;

    SnowMesh snowMesh;

    SimpleQuad fragmentPerPixelQuad{};
    fragmentPerPixelQuad.position = glm::vec3(0, 0, 0);
    fragmentPerPixelQuad.scale = glm::vec3(2, 2, 1.0f);

    // Quad used for visualization textures
    SimpleQuad displayTextureQuad{};
    float displayTextureQuadSize = 600.0f;
    displayTextureQuad.position = glm::vec3(WINDOW_WIDTH/2.0f - displayTextureQuadSize/2.0f, -WINDOW_HEIGHT/2.0f + displayTextureQuadSize/2.0f, 0);
    displayTextureQuad.scale = glm::vec3(displayTextureQuadSize, displayTextureQuadSize, 1.0f);

    // Create FBOs
    FBOWrapper fboDepth = createFBOForDepth();
    FBOWrapper fboNormals = createFrameBufferSingleTexture(depthSize);
    FBOWrapper fboNormals2 = createFrameBufferSingleTexture(depthSize);

    // Generate SSBO on GPU
    GLuint ssbo;
    glGenBuffers(1, &ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, depthSize * depthSize * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);

    double previousTimeFPS = glfwGetTime();
    double previousTime = glfwGetTime();
    int frameCounter = 0;
    float dt = 0;
    int iteration = 0;

    while (!glfwWindowShouldClose(window)) {
        glClearColor(164/255.0, 185/255.0, 196/255.0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Calculate deltaTime and FPS
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

        updateCamera(dt);
        glm::vec3 lookAtCenter = camera.position + camera.forward * 10.0f;
        glm::mat4 cameraMatrix = glm::lookAt(camera.position, lookAtCenter, camera.up);

        if (iteration == 0) {
            // Create depth texture
            glViewport(0, 0, depthSize, depthSize);
            glBindFramebuffer(GL_FRAMEBUFFER, fboDepth.fbo);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            renderModel(terrainModel, shaderProgramDepth, orthoProjectionForDepthTexture, depthCameraMatrix, fboDepth.texture, toDepthTextureSpace);
            renderModel(sphere, shaderProgramDepth, orthoProjectionForDepthTexture, depthCameraMatrix, fboDepth.texture, toDepthTextureSpace);
            renderModel(cottage, shaderProgramDepth, orthoProjectionForDepthTexture, depthCameraMatrix, fboDepth.texture, toDepthTextureSpace);
 
            // Create normal map from depth texture
            fragmentPerPixelQuad.textureId = fboDepth.texture;
            glViewport(0, 0, depthSize, depthSize);

            glBindFramebuffer(GL_FRAMEBUFFER, fboNormals.fbo);
            fragmentPerPixelQuad.render(shaderCalcNormals, identity, identity);
            fragmentPerPixelQuad.textureId = fboNormals.texture;

            // Run normal map through low pass filter 20 times
            for (int i = 0; i < 20; i++) {
                glBindFramebuffer(GL_FRAMEBUFFER, fboNormals2.fbo);
                fragmentPerPixelQuad.textureId = fboNormals.texture;
                fragmentPerPixelQuad.render(lowpassProgram, identity, identity);

                glBindFramebuffer(GL_FRAMEBUFFER, fboNormals.fbo);
                fragmentPerPixelQuad.textureId = fboNormals2.texture;
                fragmentPerPixelQuad.render(lowpassProgram, identity, identity);
            }

            glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // Build snow mesh
            snowMesh = buildMesh(fboDepth.fbo, fboDepth.texture, glm::vec3());
        }

        // Update particles on GPU
        updateParticlesOnGPU(ps, particleShader, toDepthTextureSpace, fboDepth.texture, fboNormals.texture, dt); // updates particle system
        updateParticlesOnGPU(ps2, particleShader, toDepthTextureSpace, fboDepth.texture, fboNormals.texture, dt); // updates particle system
        updateParticlesOnGPU(ps3, particleShader, toDepthTextureSpace, fboDepth.texture, fboNormals.texture, dt); // updates particle system
        updateParticlesOnGPU(ps4, particleShader, toDepthTextureSpace, fboDepth.texture, fboNormals.texture, dt); // updates particle system

        // Render the world to the main framebuffer
        renderModel(terrainModel, phongShader, perspective, cameraMatrix, fboDepth.texture, toLightSpace);
        renderModel(sphere, phongShader, perspective, cameraMatrix, fboDepth.texture, toLightSpace);
        renderModel(cottage, phongShader, perspective, cameraMatrix, fboDepth.texture, toLightSpace);

        // Render snow mesh
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        renderSnowMesh(snowMesh, fboNormals.texture, snowProgram, toDepthTextureSpace, perspective, cameraMatrix);
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);

        // Render particles
        glEnable(GL_BLEND);
        glDepthMask(GL_FALSE);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        renderInstanced(ps, shaderProgramInstanced, cameraMatrix, perspective);
        renderInstanced(ps2, shaderProgramInstanced, cameraMatrix, perspective);
        renderInstanced(ps3, shaderProgramInstanced, cameraMatrix, perspective);
        renderInstanced(ps4, shaderProgramInstanced, cameraMatrix, perspective);
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);

        // Display low-pass filtered normal map
        // displayTextureQuad.textureId = fboNormals.texture;
        // displayTextureQuad.render(shaderProgram, orthoProjection, identity);
        
        // Display depth texture
        // displayTextureQuad.textureId = fboDepth.texture;
        // displayTextureQuad.render(shaderProgram, orthoProjection, identity);

        glfwPollEvents();
        glfwSwapBuffers(window);
        iteration++;
    }

    delete heightValues;

    glfwTerminate();
}