#include <cmath>

#include "glm/vec3.hpp"
#include "glew.h"

extern int depthSize;

class SnowMesh {
public:
    GLuint vao;
    GLuint indexBuffer;
    int numIndices;
};

void renderSnowMesh(SnowMesh &mesh, GLuint normalMap, GLuint textureId,  GLuint vertexOffsetTexture, GLuint shaderProgram, glm::mat4 toLightSpace,  glm::mat4 projection, glm::mat4 camera) {
    glUseProgram(shaderProgram);

    // projection is 20 units in each direction. And this is put into 1024 by 1024 pixels.
    // So 1 pixel should be 20/1024  = 0.01953125

    float xScale = 20.0f/(float)depthSize;
    float yScale = 20.0f/1.0f;

    glm::vec3 scaleV = glm::vec3(xScale, yScale, xScale);

    glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(-10, -10+0.005f, 10));
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), scaleV);
    glm::mat4 rotation = glm::mat4(1.0f);
    glm::mat4 modelToWorld = translate * scale;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, vertexOffsetTexture);
    glUniform1i(glGetUniformLocation(shaderProgram, "snowOffsetTexture"), 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, normalMap);
    glUniform1i(glGetUniformLocation(shaderProgram, "normalMap"), 2);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelToWorld"), 1, GL_FALSE, glm::value_ptr(modelToWorld));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "worldToView"), 1, GL_FALSE, glm::value_ptr(camera));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "toLightSpace"), 1, GL_FALSE, glm::value_ptr(toLightSpace));

    glBindVertexArray(mesh.vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexBuffer);

    glDrawElements(GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_INT, (void*)0);
}

// Since the heightmap comes from a texture (0, 0) is probably in the lower left corner.
SnowMesh heightmapToSnowMesh(GLfloat *heightmap)  {
    const int width = depthSize;
    const int height = depthSize;

    const int verticesSize = width * height * 4;
    float* vertices = new float[verticesSize];

    const int texturesSize = width * height * 2;
    float* textures = new float[texturesSize];

    const int indicesSize = (width - 1) * (height - 1) * 6;
    int* indices = new int[indicesSize];


    for (int h = 0; h < height; h++) {
        for (int w = 0; w < width; w++) {
            vertices[(h * width + w) * 4 + 0] = w;
            vertices[(h * width + w) * 4 + 1] = 1-heightmap[h * width + w];
            vertices[(h * width + w) * 4 + 2] = -h;
            vertices[(h * width + w) * 4 + 3] = 1;

            textures[(h * width + w) * 2] = ((float)w / (float)(width - 1));
			textures[(h * width + w) * 2 + 1] = (1 - (float)h / (float)(height - 1));
        }
    }

    int arrayIndex = 0;
    for (int h = 0; h < height; h++) {
        for (int w = 0; w < width; w++) {
            if (w != width - 1 && h != height - 1) {
				int vertexIndex = h * width + w;

                float limit = 0.002f;
                float heightdiff1 = std::abs(heightmap[vertexIndex] - heightmap[vertexIndex + width]);
                float heightdiff2 = std::abs(heightmap[vertexIndex] - heightmap[vertexIndex + 1]);
                float heightdiff3 = std::abs(heightmap[vertexIndex + width + 1] - heightmap[vertexIndex + 1]);
                float heightdiff4 = std::abs(heightmap[vertexIndex + width + 1] - heightmap[vertexIndex + width]);

                float heightdiff5 = std::abs(heightmap[vertexIndex + width + 1] - heightmap[vertexIndex]);
                
                if (heightdiff5 < limit && heightdiff1 < limit && heightdiff2 < limit && heightdiff3 < limit && heightdiff4 < limit) {
                    indices[arrayIndex] = vertexIndex;
                    indices[arrayIndex + 1] = vertexIndex + 1;
                    indices[arrayIndex + 2] = vertexIndex + width;
                    arrayIndex += 3;
                    indices[arrayIndex] = vertexIndex + 1;
				    indices[arrayIndex + 1] = vertexIndex + width + 1;
				    indices[arrayIndex + 2] = vertexIndex + width;
                    arrayIndex += 3;

                }

                if (heightdiff1 > limit) {
                    vertices[vertexIndex * 4 + 3]=0;
                    vertices[(vertexIndex + width) * 4 + 3]=0;
                }
                if (heightdiff2 > limit) {
                    vertices[vertexIndex * 4 + 3]=0;
                    vertices[(vertexIndex + 1) * 4 + 3]=0;
                }
                if (heightdiff3 > limit) {
                    vertices[(vertexIndex + 1) * 4 + 3]=0;
                    vertices[(vertexIndex + width + 1) * 4 + 3]=0;
                }
                if (heightdiff4 > limit) {
                    vertices[(vertexIndex + width) * 4 + 3]=0;
                    vertices[(vertexIndex + width + 1) * 4 + 3]=0;
                }
        


			}
        }
    }

    GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Vertex
	GLuint vertexBuffer = 0;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verticesSize, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

    // Texture
	GLuint textureBuffer = 0;
	glGenBuffers(1, &textureBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * texturesSize, textures, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

    // Index
	GLuint indexBuffer = 0;
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indicesSize, indices, GL_STATIC_DRAW);

	SnowMesh m;
	m.vao = vao;
	m.numIndices = indicesSize;
    m.indexBuffer = indexBuffer;

    delete vertices;
    delete textures;
    delete indices;

	return m;
}

SnowMesh buildMesh(GLuint fboId, GLuint textureId, glm::vec3 referencePoint) {

    glBindFramebuffer(GL_READ_FRAMEBUFFER, fboId);

    int x = 0;
    int y = 0;
    int width = depthSize;
    int height = depthSize;

    int pixelSize = 32 * 4; // was 16 * 4
    GLfloat *data = (GLfloat*) malloc(pixelSize * width * height);
    //glReadPixels(x, y, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, data);

    glReadPixels(x, y, width, height, GL_RED, GL_FLOAT, data);


    // also try GL_DEPTH_COMPONENT16
    

  
    // std::cout << "before" << std::endl;
    SnowMesh m = heightmapToSnowMesh(data);
    // std::cout << "after" << std::endl;
    return m;

}