#pragma once

#include "glm/vec3.hpp"

GLuint createParticleVao() {
    float points[] = {
        0.5f,  0.5f,  0.0f,
        0.5f, -0.5f,  0.0f,
        -0.5f, -0.5f,  0.0f,

        0.5f,  0.5f,  0.0f,
        -0.5f, 0.5f,  0.0f,
        -0.5f, -0.5f,  0.0f,
    };

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 2*9 * sizeof(float), points, GL_STATIC_DRAW);

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    return vao;
}

class Particle {
public:
    glm::vec3 position;
    glm::vec3 scale;
    GLuint vao;

    Particle() {
        position = glm::vec3(0, 0, 0);
        scale = glm::vec3(1, 1, 1);   
        vao = createParticleVao();
    }
};

void renderParticle(Particle& b, GLuint shaderProgram, glm::mat4 camera, glm::mat4 perspective) {
    glUseProgram(shaderProgram);
   

    glm::mat4 translate = glm::translate(glm::mat4(1.0f), b.position);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), b.scale);
    glm::mat4 modelToWorld = translate * scale;
    glm::mat4 modelToView = perspective * camera * modelToWorld;

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelToWorld"), 1, GL_FALSE, glm::value_ptr(modelToView));

    glBindVertexArray(b.vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

class ParticleSystem {
public: 
    GLuint fbo;
    GLuint texture;
    GLuint texture2;
    GLuint quadVAO;
};

void updateParticleSystem(ParticleSystem& system, GLuint updateParticlesShader) {

    glBindFramebuffer(GL_FRAMEBUFFER, system.fbo);
    glViewport(0,0,1024,768); // do we need this?

    glUseProgram(updateParticlesShader);
    glBindFramebuffer(GL_FRAMEBUFFER, system.fbo);

    glBindVertexArray(system.quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);


    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

ParticleSystem createFrameBuffer() {
    GLuint fbo = 0;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);



    // Draw buffer #1
    GLuint renderedTexture;
    {
        glGenTextures(1, &renderedTexture);
        glBindTexture(GL_TEXTURE_2D, renderedTexture);

        const int w = 100;
        const int h = 100;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);
        //GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        //glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
    }

    // Draw buffer #2
    GLuint renderedTexture2;
    {
        glGenTextures(1, &renderedTexture2);
        glBindTexture(GL_TEXTURE_2D, renderedTexture2);

        const int w = 100;
        const int h = 100;

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

    ParticleSystem particleSystem{};
    particleSystem.fbo = fbo;
    particleSystem.texture = renderedTexture;
    particleSystem.texture2 = renderedTexture2;
    particleSystem.quadVAO = createParticleVao();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return particleSystem;
}
