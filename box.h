#pragma once

#include "particle.h"
#include "glm/vec3.hpp"

float randomBetween(int min, int max) {
    return (rand() % (max * 1000))/1000.0f;
}

GLuint genEmptyTextureForBox() {
    GLuint renderedTexture;
    glGenTextures(1, &renderedTexture);
    glBindTexture(GL_TEXTURE_2D, renderedTexture);

    const int w = 100;
    const int h = 100;
    const int length = w * h * 4;
    float pixels[length];

    for (int i = 0; i < length; i += 4) {
        // if (i < length / 2) {
            pixels[i] = 0; //randomBetween(0, 10) - 5.0f;
            pixels[i + 1] = 0;//randomBetween(0, 10) - 5.0f;
            pixels[i + 2] = 0; //randomBetween(0, 10) - 5.0f;
            pixels[i + 3] = randomBetween(0, 20) * 0.1f;
        // } else {
    
        // }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, &pixels[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return renderedTexture;
}

GLuint genEmptyTextureForBoxVelocity() {
    GLuint renderedTexture;
    glGenTextures(1, &renderedTexture);
    glBindTexture(GL_TEXTURE_2D, renderedTexture);

    const int w = 100;
    const int h = 100;
    const int length = w * h * 4;
    float pixels[length];

    for (int i = 0; i < length; i += 4) {
        // if (i < length / 2) {
            pixels[i] = (randomBetween(0, 1) - 0.5f) * 2;
            pixels[i + 1] = randomBetween(0, 1) *0.4f + 2;
            pixels[i + 2] = (randomBetween(0, 1) - 0.5f) * 2;
            pixels[i + 3] = 0; //;
        // } else {
    

        // }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, &pixels[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return renderedTexture;
}


ParticleSystem particleSystem;

GLuint velocity;
GLuint initialV;

void initStuff() {
    particleSystem = createFrameBuffer();
    velocity = genEmptyTextureForBoxVelocity();
    initialV = genEmptyTextureForBoxVelocity();
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

class Box {
public:
    glm::vec3 position;
    glm::vec3 scale;
    GLuint vao;
    GLuint textureId;

    Box() {
        position = glm::vec3(0, 0, 0);
        scale = glm::vec3(1, 1, 1);   
        vao = createVao();
        textureId = genEmptyTextureForBox();
    }
};

void renderBox(Box& b, GLuint shaderProgram, GLuint shaderProgramParticle, glm::mat4 camera, glm::mat4 perspective) {


    // ----------------------- DO STUFF HERE TO UPDATE TEXTURE
    glBindFramebuffer(GL_FRAMEBUFFER, particleSystem.fbo);
    glUseProgram(shaderProgramParticle);

    glm::mat4 translate = glm::translate(glm::mat4(1.0f), b.position);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), b.scale);
    glm::mat4 modelToWorld = translate * scale;
    glm::mat4 modelToView = scale;

    glUniformMatrix4fv(glGetUniformLocation(shaderProgramParticle, "modelToWorld"), 1, GL_FALSE, glm::value_ptr(modelToView));

    glViewport(0, 0, 100, 100);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, b.textureId);
    glUniform1i(glGetUniformLocation(shaderProgramParticle, "tex"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, velocity);
    glUniform1i(glGetUniformLocation(shaderProgramParticle, "texVelocity"), 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, initialV);
    glUniform1i(glGetUniformLocation(shaderProgramParticle, "initVelocity"), 2);

    glBindVertexArray(b.vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);


    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, b.textureId, 0);
    GLuint temp = b.textureId;
    b.textureId = particleSystem.texture;
    particleSystem.texture = temp;

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, velocity, 0);
    GLuint temp2 = velocity;
    velocity = particleSystem.texture2;
    particleSystem.texture2 = temp2;


    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    // ----------------------- END DO STUFF

    // glUseProgram(shaderProgram);


    

    // translate = glm::translate(glm::mat4(1.0f), b.position);
    // scale = glm::scale(glm::mat4(1.0f), b.scale);
    // modelToWorld = translate * scale;
    // modelToView = perspective * camera * modelToWorld;

    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, b.textureId);
    // glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0);

    // glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelToWorld"), 1, GL_FALSE, glm::value_ptr(modelToView));

    // glBindVertexArray(b.vao);
    // glDrawArrays(GL_TRIANGLES, 0, 6);
}

void renderInstanced(Box& b, GLuint shaderProgram, glm::mat4 camera, glm::mat4 perspective) { 
    glUseProgram(shaderProgram);

    b.scale = glm::vec3(0.03f,0.03f,1.0f);
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), b.scale);
    glm::mat4 rotation = glm::mat4(1.0f);
    glm::mat4 modelToWorld = scale;

    glm::mat4 modelToView = perspective;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, b.textureId);
    glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "perspective"), 1, GL_FALSE, glm::value_ptr(perspective));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "translation"), 1, GL_FALSE, glm::value_ptr(translate));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "scale"), 1, GL_FALSE, glm::value_ptr(scale));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(camera));

    glBindVertexArray(b.vao);

    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 10000);

    b.scale = glm::vec3(1.0f,1.0f,1.0f);
}