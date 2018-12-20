class SimpleQuad {
public:
    glm::vec3 position;
    glm::vec3 scale;
    GLuint vao;
    GLuint indexBuffer;
    GLuint textureId;

    SimpleQuad() {
        position = glm::vec3(0, 0, 0);
        scale = glm::vec3(1, 1, 1);

        textureId = genTextureForQuad();
        createVAO();
        createIndexBuffer();
    }

    void createVAO();
    void createIndexBuffer();
    GLuint genTextureForQuad();
    void render(GLuint shader, glm::mat4 projection, glm::mat4 camera);
};

void SimpleQuad::render(GLuint shaderProgram, glm::mat4 projection, glm::mat4 camera) {
    glUseProgram(shaderProgram);

    glm::mat4 translate = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), this->scale);
    glm::mat4 rotation = glm::mat4(1.0f);
    glm::mat4 modelToWorld = translate * scale;
    glm::mat4 modelToClip = projection * camera * modelToWorld;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelToClip"), 1, GL_FALSE, glm::value_ptr(modelToClip));

    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
}

GLuint SimpleQuad::genTextureForQuad() {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    const int w = 100;
    const int h = 100;
    const int length = w * h * 4;
    float pixels[length];

    for (int i = 0; i < length; i += 4) {
        pixels[i] = 1;
        pixels[i + 1] = 0;
        pixels[i + 2] = 0; 
        pixels[i + 3] = 1;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, &pixels[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return texture;
}

void SimpleQuad::createVAO() {
    float points[] = {
        -0.5f,  0.5f,  0.0f,
        0.5f,  0.5f,  0.0f,
        0.5f, -0.5f,  0.0f,
        -0.5f, -0.5f,  0.0f,
    };

    float texcoord[] = {
        0.0f, 1.0f,  0.0f,
        1.0f, 1.0f,  0.0f,
        1.0f, 0.0f,  0.0f,
        0.0f, 0.0f,  0.0f,
    };

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), points, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    GLuint vboTex = 0;
    glGenBuffers(1, &vboTex);
    glBindBuffer(GL_ARRAY_BUFFER, vboTex);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), texcoord, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vboTex);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    this->vao = vao;
}

void SimpleQuad::createIndexBuffer() {
    unsigned int indices[] {
        0, 1, 3,
        1, 2, 3,
    };

    GLuint indexBuffer;
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    this->indexBuffer = indexBuffer;
}