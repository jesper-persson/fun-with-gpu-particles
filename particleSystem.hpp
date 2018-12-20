#pragma once

#include "glew.h"
#include "glm/vec3.hpp"

extern const int depthSize;

// numDecimals = 0 means we generate integers.
// numDecimals = 1 means we generate with one decimal.
float randomBetween(int min, int max, int numDecimals = 0) {
    int multiplier = pow(10, numDecimals);
    max = max * multiplier;
    min = min * multiplier;
    float num = (rand() / (double)RAND_MAX) * (max-min) + min;
    return num / multiplier;
}

// Creates 3d texture, to be used as force field.
GLuint createTextureForForceField() {
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_3D, textureId);

    const int depth = 2;
    const int width = 2;
    const int height = 2;

    const int length = width * height * depth * 4 ;
    GLfloat pixels[length];

    for (int i = 0; i < length; i += 4) {
        pixels[i] = 0;
        pixels[i + 1] = 0;
        pixels[i + 2] = 0;
        pixels[i + 3] = 0;
    }

    float speed = 1.0f;
    pixels[0] = speed;
    pixels[0 + 1] = 0;
    pixels[0 + 2] = 0;
    pixels[0 + 3] = 0;

    pixels[4] = -speed/4.0f;
    pixels[4 + 1] = speed;
    pixels[4 + 2] = 0;
    pixels[4 + 3] = 0;

    pixels[8] = 0;
    pixels[8 + 1] = -speed;
    pixels[8 + 2] = 0;
    pixels[8 + 3] = 0;

    pixels[12] = -speed;
    pixels[12 + 1] = 0;
    pixels[12 + 2] = 0;
    pixels[12 + 3] = 0;

    pixels[16] = speed;
    pixels[16 + 1] = 0;
    pixels[16 + 2] = 0;
    pixels[16 + 3] = 0;

    pixels[20] = 0;
    pixels[20 + 1] = speed;
    pixels[20 + 2] = 0;
    pixels[20 + 3] = 0;

    pixels[24] = 0;
    pixels[24 + 1] = -speed;
    pixels[24 + 2] = 0;
    pixels[24 + 3] = 0;

    pixels[28] = 0;
    pixels[28 + 1] = -speed;
    pixels[28 + 2] = 0;
    pixels[28 + 3] = 0;

    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, width, height, depth, 0, GL_RGBA, GL_FLOAT, &pixels[0]);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return textureId;   
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

GLuint genVelocityTexture(const int textureSize) {
    const int w = textureSize;
    const int h = textureSize;
    const int length = w * h * 4;
    float pixels[length];

    for (int i = 0; i < length; i += 4) {
        pixels[i] = randomBetween(-1, 1, 8) * 6.0f;
        pixels[i + 1] = randomBetween(-3, -2, 8) / 1.5f * 0.7f;
        pixels[i + 2] = randomBetween(-1, 1, 8) * 6.0f;
        pixels[i + 3] = 0;
    }

    return genTextureWithData(w, h, pixels);
}

GLuint genPositionTexture(const int textureSize) {
    const int w = textureSize;
    const int h = textureSize;
    const int length = w * h * 4;
    float pixels[length];

    for (int i = 0; i < length; i += 4) {
        pixels[i] = randomBetween(-10, 10, 3) / 1.0f;
        pixels[i + 1] = randomBetween(2, 5, 2);
        pixels[i + 2] = randomBetween(-10,10, 3) / 1.0f;
        pixels[i + 3] = randomBetween(15, 20, 2);
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
    }
    GLenum DrawBuffers[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, DrawBuffers); // "1" is the size of DrawBuffers

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
    glm::vec3 centerPosition;
    GLuint vao;

    GLuint velocity;
    GLuint initialVelocity;

    GLuint position;
    GLuint initialPosition;

    GLuint colorTexture;

    ParticleSystemFBO particleSystem;

    int numParticles;
    int sqrtNumParticles;

    GLuint forceFieldTexture;

    ParticleSystem(int numParticles) {
        this->sqrtNumParticles = sqrt(numParticles);
        this->numParticles = this->sqrtNumParticles * this->sqrtNumParticles;

        scale = glm::vec3(1, 1, 1);
        centerPosition = glm::vec3(0, 0, 0);
        vao = createVao();

        velocity = genVelocityTexture(sqrtNumParticles);
        initialVelocity = genVelocityTexture(sqrtNumParticles);

        position = genPositionTexture(sqrtNumParticles);
        initialPosition = genPositionTexture(sqrtNumParticles);
        
        colorTexture = genWhiteTexture();

        particleSystem = createFrameBufferForParticles(sqrtNumParticles);

        forceFieldTexture = createTextureForForceField();
    }
};

void updateParticlesOnGPU(ParticleSystem& ps, GLuint shaderProgramParticle, glm::mat4& toDepthTextureSpace, GLuint depthTextureId, GLuint normalTexture, float dt) {
    glm::vec3 tempScale = ps.scale;
    ps.scale = glm::vec3(1.0f, 1.0f, 1.0f);

    glBindFramebuffer(GL_FRAMEBUFFER, ps.particleSystem.fbo);
    glUseProgram(shaderProgramParticle);

    glm::mat4 translate = glm::translate(glm::mat4(1.0f), ps.centerPosition);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), ps.scale);
    glm::mat4 modelToClip = translate * scale;

    glUniformMatrix4fv(glGetUniformLocation(shaderProgramParticle, "modelToClip"), 1, GL_FALSE, glm::value_ptr(modelToClip));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgramParticle, "toDepthTextureSpace"), 1, GL_FALSE, glm::value_ptr(toDepthTextureSpace));

    glViewport(0, 0, ps.sqrtNumParticles, ps.sqrtNumParticles);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ps.position);
    glUniform1i(glGetUniformLocation(shaderProgramParticle, "positionTexture"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ps.velocity);
    glUniform1i(glGetUniformLocation(shaderProgramParticle, "velocityTexture"), 1);
    
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, ps.initialVelocity);
    glUniform1i(glGetUniformLocation(shaderProgramParticle, "initialVelocityTexture"), 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, ps.initialPosition);
    glUniform1i(glGetUniformLocation(shaderProgramParticle, "initialPositionTexture"), 3);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, depthTextureId);
    glUniform1i(glGetUniformLocation(shaderProgramParticle, "texDepth"), 4);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_3D, ps.forceFieldTexture);
    glUniform1i(glGetUniformLocation(shaderProgramParticle, "forceFieldTexture"), 5);

    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, normalTexture);
    glUniform1i(glGetUniformLocation(shaderProgramParticle, "normalTexture"), 6);

    glUniform1f(glGetUniformLocation(shaderProgramParticle, "dt"), dt);

    glBindVertexArray(ps.vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Swap frame buffer textures (previous output becomes next input)
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, ps.position, 0);
    GLuint temp = ps.position;
    ps.position = ps.particleSystem.outputPositionTexture;
    ps.particleSystem.outputPositionTexture = temp;

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, ps.velocity, 0);
    GLuint temp2 = ps.velocity;
    ps.velocity = ps.particleSystem.outputVelocityTexture;
    ps.particleSystem.outputVelocityTexture = temp2;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    ps.scale = tempScale;
}

void renderInstanced(ParticleSystem& ps, GLuint shaderProgram, glm::mat4 camera, glm::mat4 projection) { 
    glUseProgram(shaderProgram);

    glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), ps.scale);
    glm::mat4 rotation = glm::mat4(1.0f);
    glm::mat4 modelToWorld = scale;

    glm::mat4 modelToView = projection;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ps.position);
    glUniform1i(glGetUniformLocation(shaderProgram, "particlePositions"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ps.colorTexture);
    glUniform1i(glGetUniformLocation(shaderProgram, "diffuseTexture"), 1);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "translation"), 1, GL_FALSE, glm::value_ptr(translate));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "scale"), 1, GL_FALSE, glm::value_ptr(scale));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(camera));
    glUniform1i(glGetUniformLocation(shaderProgram, "textureSize"), ps.sqrtNumParticles);

    glBindVertexArray(ps.vao);

    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, ps.numParticles);
}

struct FBOWrapper {
    GLuint fbo;
    GLuint texture;
};

FBOWrapper createFBOForDepth() {
    GLuint fboId = 0;
    glGenFramebuffers(1, &fboId);
    glBindFramebuffer(GL_FRAMEBUFFER, fboId);

    GLuint depthTexture;
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, depthSize, depthSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    // This parameters did not work on one computer
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

    glDrawBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Could not create frame buffer" << std::endl;
    }

    FBOWrapper fbo;
    fbo.fbo = fboId;
    fbo.texture = depthTexture;
    return fbo;
}

FBOWrapper createFrameBufferSingleTexture(int textureSize) {
    GLuint fboId = 0;
    glGenFramebuffers(1, &fboId);
    glBindFramebuffer(GL_FRAMEBUFFER, fboId);

    GLuint renderedTexture;
    {
        glGenTextures(1, &renderedTexture);
        glBindTexture(GL_TEXTURE_2D, renderedTexture);

        const int w = textureSize;
        const int h = textureSize;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Could not create frame buffer" << std::endl;
    }

    FBOWrapper fbo{};
    fbo.fbo = fboId;
    fbo.texture = renderedTexture;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return fbo;
}