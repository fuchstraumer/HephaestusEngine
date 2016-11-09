#pragma once
#ifndef LODE_TEXTURE_H
#define LODE_TEXTURE_H
#include "lodepng.h"
#define GLEW_STATIC
#include <GL/glew.h>
#include <string>
using uint = unsigned int;
// Builds and loads an OpenGL 3D texture for use as a 2D texture array.
// To use in a program, call glBindTexture(texture->GL_Handle)
class TextureArray {
public:
	GLuint GL_Handle;
	// The input vector contains the filenames to use for importing
	TextureArray(std::vector<std::string> filelist, uint textureDims) {
		glGenTextures(1, &this->GL_Handle);
		this->depth = filelist.size();
		this->fileList = filelist;
		this->TextureDimensions = textureDims;
	}
	// Builds the texture array: pass in an active texture unit for this 
	// array to assign to, and then active the array by calling glBindTexture(
	// TextureArray->GL_Handle);
	void BuildTextureArray(GLenum activetexture);
	uint TextureDimensions;
private:
	std::vector<unsigned char*> textureData;
	std::vector<std::string> fileList;
	std::size_t depth; // Sets amount of slots to reserve in the texture array
};


inline void TextureArray::BuildTextureArray(GLenum activetexture) {
	for (auto str : this->fileList) {
		unsigned char* data;
		lodepng_decode32_file(&data, &this->TextureDimensions, &this->TextureDimensions, 
			str.data());
		this->textureData.push_back(data);
	}
	GLenum err;
	glGenTextures(1, &this->GL_Handle);
	glActiveTexture(activetexture);
	err = glGetError();
	glBindTexture(GL_TEXTURE_2D_ARRAY,this->GL_Handle);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, this->TextureDimensions, this->TextureDimensions, 
		this->depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	for (uint i = 0; i < this->depth; ++i) {
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, this->TextureDimensions, this->TextureDimensions, 1, 
			GL_RGBA, GL_UNSIGNED_BYTE, this->textureData[i]);
	}
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// Set texture wrapping to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	glBindTexture(GL_TEXTURE,0);
}

#endif // !LODE_TEXTURE_H
