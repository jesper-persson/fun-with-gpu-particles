#pragma once

#include "glew.h"
#include "glm/vec3.hpp"

float randomBetween(int min, int max) {
    return (rand() / (float)RAND_MAX) * (max-min) + min;
}

// Helper function to create texture of given size and data.
GLuint genTextureWithData(const int width, const int height, float *data) {
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    const int length = width * height * 4;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, &data[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return textureId;    
}

GLuint genWhiteTexture() {
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    const int w = 100;
    const int h = 100;
    const int length = w * h * 4;
    float pixels[length];

    for (int i = 0; i < length; i += 4) {
        pixels[i] = 1.0f;
        pixels[i + 1] = 1.0f;
        pixels[i + 2] = 1.0f;
        pixels[i + 3] = 1.0f;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, &pixels[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return textureId;
}

GLuint genVelocityTexture(int textureSize) {
    const int w = textureSize;
    const int h = textureSize;
    const int length = w * h * 4;
    float pixels[length];

    for (int i = 0; i < length; i += 4) {
        pixels[i] = randomBetween(-10, 10) / 50.0f;
        pixels[i + 1] = -randomBetween(1, 10) / 20.0f;;
        pixels[i + 2] = randomBetween(-10, 10) / 50.0f;
        pixels[i + 3] = 0;
    }

    return genTextureWithData(w, h, pixels);
}

GLuint genPositionTexture(int textureSize) {
    const int w = textureSize;
    const int h = textureSize;
    const int length = w * h * 4;
    float pixels[length];

    for (int i = 0; i < length; i += 4) {
        pixels[i] = randomBetween(-10, 10) / 5.0f;
        pixels[i + 1] = randomBetween(-10, 10) / 100.0f;
        pixels[i + 2] = randomBetween(-10, 10) / 5.0f;
        pixels[i + 3] = randomBetween(10, 100) / 5.0f; // Time alive
    }

    return genTextureWithData(w, h, pixels);
}

GLuint createVao() {
    float points[] = {
        1.0f,  1.0f,  0.0f,
        1.0f, -1.0f,  0.0f,
        -1.0f, -1.0f,  0.0f,

        1.0f,  1.0f,  0.0f,
        -1.0f, 1.0f,  0.0f,
        -1.0f, -1.0f,  0.0f,
    };

    float texcoord[] = {
        1.0f, 1.0f,  0.0f,
        1.0f, 0.0f,  0.0f,
        0.0f, 0.0f,  0.0f,

        1.0f, 1.0f,  0.0f,
        0.0f, 1.0f,  0.0f,
        0.0f, 0.0f,  0.0f,
    };

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 2*9 * sizeof(float), points, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    GLuint vboTex = 0;
    glGenBuffers(1, &vboTex);
    glBindBuffer(GL_ARRAY_BUFFER, vboTex);
    glBufferData(GL_ARRAY_BUFFER, 2*9 * sizeof(float), texcoord, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vboTex);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    return vao;
}

class ParticleSystemFBO {
public: 
    GLuint fbo;
    GLuint outputPositionTexture;
    GLuint outputVelocityTexture;
};

ParticleSystemFBO createFrameBufferForParticles(int textureSize) {
    GLuint fbo = 0;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Draw buffer #1 (used for positions)
    GLuint renderedTexture;
    {
        glGenTextures(1, &renderedTexture);
        glBindTexture(GL_TEXTURE_2D, renderedTexture);

        const int w = textureSize;
        const int h = textureSize;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);
        //GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        //glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
    }

    // Draw buffer #2 (used for velocity)
    GLuint renderedTexture2;
    {
        glGenTextures(1, &renderedTexture2);
        glBindTexture(GL_TEXTURE_2D, renderedTexture2);

        const int w = textureSize;
        const int h = textureSize;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, renderedTexture2, 0);
        GLenum DrawBuffers[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        glDrawBuffers(2, DrawBuffers); // "1" is the size of DrawBuffers
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Could not create frame buffer" << std::endl;
    }

    ParticleSystemFBO ParticleSystemFBO{};
    ParticleSystemFBO.fbo = fbo;
    ParticleSystemFBO.outputPositionTexture = renderedTexture;
    ParticleSystemFBO.outputVelocityTexture = renderedTexture2;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return ParticleSystemFBO;
}

class ParticleSystem {
public:
    glm::vec3 scale;
    GLuint vao;

    GLuint velocity;
    GLuint initialVelocity;

    GLuint position;
    GLuint initialPosition;

    GLuint colorTexture;

    ParticleSystemFBO particleSystem;

    int numParticles;
    int sqrtNumParticles;

    ParticleSystem(int numParticles) {
        this->sqrtNumParticles = sqrt(numParticles);
        this->numParticles = this->sqrtNumParticles * this->sqrtNumParticles;

        scale = glm::vec3(1, 1, 1);   
        vao = createVao();

        velocity = genVelocityTexture(sqrtNumParticles);
        initialVelocity = genVelocityTexture(sqrtNumParticles);

        position = genPositionTexture(sqrtNumParticles);
        initialPosition = genPositionTexture(sqrtNumParticles);
        
        colorTexture = genWhiteTexture();

        particleSystem = createFrameBufferForParticles(sqrtNumParticles);
    }
};

void updateParticlesOnGPU(ParticleSystem& b, GLuint shaderProgramParticle, glm::mat4 camera, glm::mat4 perspective, glm::mat4& toLightSpace, GLuint depthTextureId, float dt) {


    // ----------------------- DO STUFF HERE TO UPDATE TEXTURE
    glm::vec3 tempScale = b.scale;
    b.scale = glm::vec3(1.0f, 1.0f, 1.0f);

    glBindFramebuffer(GL_FRAMEBUFFER, b.particleSystem.fbo);
    glUseProgram(shaderProgramParticle);

    glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)); //, b.position);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), b.scale);
    glm::mat4 modelToWorld = translate * scale;
    glm::mat4 modelToView = scale;

    glUniformMatrix4fv(glGetUniformLocation(shaderProgramParticle, "modelToWorld"), 1, GL_FALSE, glm::value_ptr(modelToView));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgramParticle, "toLightSpace"), 1, GL_FALSE, glm::value_ptr(toLightSpace));

    glViewport(0, 0, b.sqrtNumParticles, b.sqrtNumParticles);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, b.position);
    glUniform1i(glGetUniformLocation(shaderProgramParticle, "positionTexture"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, b.velocity);
    glUniform1i(glGetUniformLocation(shaderProgramParticle, "velocityTexture"), 1);
    

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, b.initialVelocity);
    glUniform1i(glGetUniformLocation(shaderProgramParticle, "initialVelocityTexture"), 2);


    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, b.initialPosition);
    glUniform1i(glGetUniformLocation(shaderProgramParticle, "initialPositionTexture"), 3);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, depthTextureId);
    glUniform1i(glGetUniformLocation(shaderProgramParticle, "texDepth"), 4);

    glUniform1f(glGetUniformLocation(shaderProgramParticle, "dt"), dt);

    glBindVertexArray(b.vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);


    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, b.position, 0);
    GLuint temp = b.position;
    b.position = b.particleSystem.outputPositionTexture;
    b.particleSystem.outputPositionTexture = temp;

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, b.velocity, 0);
    GLuint temp2 = b.velocity;
    b.velocity = b.particleSystem.outputVelocityTexture;
    b.particleSystem.outputVelocityTexture = temp2;


    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    b.scale = tempScale;

    // ----------------------- END DO STUFF

}

void renderInstanced(ParticleSystem& b, GLuint shaderProgram, glm::mat4 camera, glm::mat4 perspective) { 
    glUseProgram(shaderProgram);

    glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), b.scale);
    glm::mat4 rotation = glm::mat4(1.0f);
    glm::mat4 modelToWorld = scale;

    glm::mat4 modelToView = perspective;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, b.position);
    glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, b.colorTexture);
    glUniform1i(glGetUniformLocation(shaderProgram, "tex1"), 1);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "perspective"), 1, GL_FALSE, glm::value_ptr(perspective));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "translation"), 1, GL_FALSE, glm::value_ptr(translate));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "scale"), 1, GL_FALSE, glm::value_ptr(scale));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(camera));
    glUniform1i(glGetUniformLocation(shaderProgram, "textureSize"), b.sqrtNumParticles);

    glBindVertexArray(b.vao);

    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, b.numParticles);
}

struct DepthFBO {
    GLuint fbo;
    GLuint texture;
};

const int shadow_width = 1024; // Change in particleFragment too
const int shadow_height = 1024; // Change in particleFragment too

DepthFBO createFBOForDepth() {
    GLuint fboId = 0;
    glGenFramebuffers(1, &fboId);
    glBindFramebuffer(GL_FRAMEBUFFER, fboId);

    GLuint depthTexture;
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, shadow_width, shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

    glDrawBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Could not create frame buffer" << std::endl;
    }

    DepthFBO fbo;
    fbo.fbo = fboId;
    fbo.texture = depthTexture;
    return fbo;
}