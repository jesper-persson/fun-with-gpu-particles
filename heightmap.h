#include <memory>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader.h"

#include "glm/gtx/transform.hpp"
#include "glm/glm.hpp"
#include "glm/vec3.hpp"
#include "glew.h"

class Model {
public:
    GLuint vao;
    GLuint numIndices;
    GLuint textureId;
    GLuint indexBuffer;
    glm::vec3 position;
    glm::vec3 scale;
	glm::mat4 rotation = glm::mat4(1.0);
	int textureScale = 1;
};

void renderModel(Model &model, GLuint shaderProgram, glm::mat4 projection, glm::mat4 camera, GLuint depthTexture, glm::mat4 toLightSpace) {
    glUseProgram(shaderProgram);

    glm::mat4 translate = glm::translate(glm::mat4(1.0f), model.position);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), model.scale);
    glm::mat4 modelToWorld = translate *  model.rotation * scale;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, model.textureId);
    glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0);

    glUniform1i(glGetUniformLocation(shaderProgram, "textureScale"), model.textureScale);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glUniform1i(glGetUniformLocation(shaderProgram, "depthTexture"), 2);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelToWorld"), 1, GL_FALSE, glm::value_ptr(modelToWorld));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "worldToView"), 1, GL_FALSE, glm::value_ptr(camera));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "toLightSpace"), 1, GL_FALSE, glm::value_ptr(toLightSpace));

    glBindVertexArray(model.vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.indexBuffer);

    glDrawElements(GL_TRIANGLES, model.numIndices, GL_UNSIGNED_INT, (void*)0);
}

// Sizes given in amounts (that is size of the array)
Model modelFromVertexData(float vertexCoordinates[], int vertexCoordinatesSize, float normals[], int normalsSize, float textureCoordinates[], int textureCoordinatesSize, int indices[], int indicesSize) {
	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Vertex
	GLuint vertexBuffer = 0;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexCoordinatesSize, vertexCoordinates, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// Normals
	GLuint normalBuffer = 0;
	glGenBuffers(1, &normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * normalsSize, normals, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	// Texture
	GLuint textureBuffer = 0;
	glGenBuffers(1, &textureBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * textureCoordinatesSize, textureCoordinates, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	// Index
	GLuint indexBuffer = 0;
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indicesSize, indices, GL_STATIC_DRAW);

	Model m;
	m.vao = vao;
	m.numIndices = indicesSize;
	m.indexBuffer = indexBuffer;
	return m;
}

Model heightmapToModel(float *heightmap, int width, int height, float scaleX, float scaleY, float scaleZ, float textureScale) {
	std::unique_ptr<float> positions(new float[width * height * 3]);
	std::unique_ptr<float> normals(new float[width * height * 3]);
	std::unique_ptr<float> textureCoordinates(new float[width * height * 2]);
	std::unique_ptr<int> indices(new int[(width - 1) * (height - 1) * 6]);

	for (int z = 0; z < height; z++) {
		for (int x = 0; x < width; x++) {
			positions.get()[(z * width + x) * 3] = x * scaleX;
			positions.get()[(z * width + x) * 3 + 1] = heightmap[width * z + x] * scaleY;
			positions.get()[(z * width + x) * 3 + 2] = z * scaleZ;
			textureCoordinates.get()[(z * width + x) * 2] = ((float)x / (float)(width - 1)) * textureScale;
			textureCoordinates.get()[(z * width + x) * 2 + 1] = (1 - (float)z / (float)(height - 1)) * textureScale;

			// Calc normals
			if (x == width - 1 || z == height - 1 || x == 0 || z == 0) {
				normals.get()[(z * width + x) * 3] = 0;
				normals.get()[(z * width + x) * 3 + 1] = 1;
				normals.get()[(z * width + x) * 3 + 2] = 0;
			} else {
				glm::vec3 a = glm::vec3(x * scaleX, heightmap[width * z + x] * scaleY, z * scaleZ);
				glm::vec3 ab = a - glm::vec3((x + 1) * scaleX, heightmap[width * z + x + 1] * scaleY, z * scaleZ);
				glm::vec3 ac = a - glm::vec3(x * scaleX, heightmap[width * (z + 1) + x] * scaleY, (z + 1) * scaleZ);
				glm::vec3 normal1 = glm::normalize(glm::cross(ac, ab));
				glm::vec3 normal = normal1;
				// if (!FAST_MODE) {
					glm::vec3 ad = a - glm::vec3((x - 1) * scaleX, heightmap[width * z + x - 1] * scaleY, z * scaleZ);
					glm::vec3 ae = a - glm::vec3(x * scaleX, heightmap[width * (z - 1) + x] * scaleY, (z - 1) * scaleZ);
					glm::vec3 normal2 = glm::normalize(glm::cross(ae, ad));
					glm::vec3 af = a - glm::vec3((x - 1) * scaleX, heightmap[width * z + x - 1] * scaleY, z * scaleZ);
					glm::vec3 ag = a - glm::vec3(x * scaleX, heightmap[width * (z + 1) + x] * scaleY, (z + 1) * scaleZ);
					glm::vec3 normal3 = glm::normalize(glm::cross(af, ag));
					glm::vec3 ah = a - glm::vec3((x + 1) * scaleX, heightmap[width * z + x + 1] * scaleY, z * scaleZ);
					glm::vec3 ai = a - glm::vec3(x * scaleX, heightmap[width * (z - 1) + x] * scaleY, (z - 1) * scaleZ);
					glm::vec3 normal4 = glm::normalize(glm::cross(ah, ai));
					normal = glm::normalize(normal1 + normal2 + normal3 + normal4);
				// }
				normals.get()[(z * width + x) * 3] = normal.x;
				normals.get()[(z * width + x) * 3 + 1] = normal.y;
				normals.get()[(z * width + x) * 3 + 2] = normal.z;
			}

			// Indices
			if (x != width - 1 && z != height - 1) {
				int index = z * width + x;
				int arrayIndex = z * (width - 1) + x;
				indices.get()[arrayIndex * 6] = index;
				indices.get()[arrayIndex * 6 + 1] = index + 1;
				indices.get()[arrayIndex * 6 + 2] = index + width;
				indices.get()[arrayIndex * 6 + 3] = index + 1;
				indices.get()[arrayIndex * 6 + 4] = index + width + 1;
				indices.get()[arrayIndex * 6 + 5] = index + width;
			}
		}
	}

	return modelFromVertexData(positions.get(), width * height * 3,
							  normals.get(), width * height * 3,
							  textureCoordinates.get(), width * height * 2,
							  indices.get(), (width - 1) * (height - 1) * 6);
}



Model tinyObjLoader(std::string filename) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str());

	if (!err.empty()) {
		std::cerr << err << std::endl;
	}

	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> textures;
	std::vector<int> indices;

	// Loop over shapes
	int i = 0;
	for (size_t s = 0; s < shapes.size(); s++) {

		// Loop over faces (polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
			int fv = shapes[s].mesh.num_face_vertices[f];

			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++) {
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				vertices.push_back(attrib.vertices[3 * idx.vertex_index + 0]);
				vertices.push_back(attrib.vertices[3 * idx.vertex_index + 1]);
				vertices.push_back(attrib.vertices[3 * idx.vertex_index + 2]);
				normals.push_back(attrib.normals[3 * idx.normal_index + 0]);
				normals.push_back(attrib.normals[3 * idx.normal_index + 1]);
				normals.push_back(attrib.normals[3 * idx.normal_index + 2]);
				textures.push_back(attrib.texcoords[2 * idx.texcoord_index + 0]);
				textures.push_back(attrib.texcoords[2 * idx.texcoord_index + 1]);
				indices.push_back(i);
				i++;
			}
			index_offset += fv;

			// per-face material
			shapes[s].mesh.material_ids[f];
		}
	}

	float *vertexData = &vertices[0];
	float *normalData = &normals[0];
	float *textureData = &textures[0];
	int *indexData = &indices[0];

	return modelFromVertexData(vertexData, vertices.size(), normalData, normals.size(), textureData, textures.size(), indexData, indices.size());
}