#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include <glad/glad.h>
#include <iostream>
#include <vector>

enum class TextureType {
	Grey,
	RGB,
	Depth,
	Stencil,
	RGBD
};

class Texture {
public:
	Texture(const GLchar* texturePath, bool flip = false);
	Texture(int width, int height, TextureType type);
	Texture(int width, int height, TextureType type, std::vector<float> data);
	~Texture();

	GLuint getId();
	int getWidth();
	int getHeight();

private:
	GLuint ID;
	int m_width, m_height;
};

#endif