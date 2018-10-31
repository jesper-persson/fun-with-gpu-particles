#include "glm/vec3.hpp"

GLuint createVao() {
    // float points[] = {
    //     0.0f,  0.5f,  0.0f,
    //     0.5f, -0.5f,  0.0f,
    //     -0.5f, -0.5f,  0.0f,
    // };

    float points[] = {
        0.0f,  0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        0.0f,  0.5f,  -0.5f,
        0.5f, -0.5f,  -0.5f,
        -0.5f, -0.5f,  -0.5f,
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

class Box {
public:
    glm::vec3 position;
    glm::vec3 scale;
    GLuint vao;

    Box() {
        position = glm::vec3(0, 0, 0);
        scale = glm::vec3(1, 1, 1);   
        vao = createVao();
    }
};

void renderBox(Box& b, GLuint shaderProgram, glm::mat4 camera, glm::mat4 perspective) {
    glUseProgram(shaderProgram);

    glm::mat4 translate = glm::translate(glm::mat4(1.0f), b.position);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), b.scale);
    glm::mat4 modelToWorld = translate * scale;
    glm::mat4 modelToView = perspective * camera * modelToWorld;


    // for (int i = 0; i < 4; i++ ) {
    //     for (int j = 0 ; j < 4 ; j++) {
    //         cout << camera[j][i] << " ";
    //     }
    //     cout << endl;
    // }



    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelToWorld"), 1, GL_FALSE, glm::value_ptr(modelToView));

    glBindVertexArray(b.vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

