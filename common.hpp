#include <iostream>
#include <string>
#include <vector>
#include "lodepng/lodepng.h"
#include "glew.h"


void loadObj(std::string filename) {

}



std::vector<float>* pngTextureToFloatArray(std::string filename) {
	const char* filenameC = (const char*)filename.c_str();
	std::vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, filename);


	// std::vector<unsigned char> imageCopy(width * height * 4);
	// for (unsigned i = 0; i < height; i++) {
	// 	memcpy(&imageCopy[(height - i - 1) * width * 4], &image[i * width * 4], width * 4);
	// }

	// Since heightmap we only one channel (the red), so the other channels are removed
	std::vector<float>* imageCopy = new std::vector<float>(width * height);
	int newBufferIndex = 0;
	int oldBufferIndex = 0;
	for (newBufferIndex = 0; newBufferIndex < height * width; newBufferIndex++, oldBufferIndex += 4) {
		(*imageCopy)[newBufferIndex]  = ( (float)image[oldBufferIndex] );
	}

	std::cout << (*imageCopy)[0] << std::endl;
	std::cout << "adsad" << (*imageCopy)[0] << std::endl;

	return imageCopy;
}



GLuint loadPNGTexture(std::string filename) {
	const char* filenameC = (const char*)filename.c_str();
	std::vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, filename);

	std::vector<unsigned char> imageCopy(width * height * 4);
	for (unsigned i = 0; i < height; i++) {
		memcpy(&imageCopy[(height - i - 1) * width * 4], &image[i * width * 4], width * 4);
	}

	GLuint texId;
	glGenTextures(1, &texId);
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &imageCopy[0]);
	glGenerateMipmap(GL_TEXTURE_2D);
	return texId;
}