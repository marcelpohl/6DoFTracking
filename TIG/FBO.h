#ifndef FBO_H
#define FBO_H

#include <glad/glad.h>
#include <vector>
#include "Texture.h"

class FBO
{
public:
	FBO(int width, int height);
	~FBO();

	void attachColorTexture(unsigned int quantity);
	void attachColorTexture(const GLchar* path, bool flip);
	void attachDepthTexture();
	void attachDepthTexture(const GLchar* path, bool flip);
	void attachStencilTexture();
	void removeColorTextures();
	void removeDepthTexture();
	void removeStencilTexture();

	void reset();
	void resize(int width, int height);

	void bind();
	void unbind();

	GLuint getColorTexture(unsigned int pos);
	GLuint getDepthTexture();
	GLuint getStencilTexture();

private:
	int m_width, m_height;
	GLuint m_FboHandle;
	std::vector<Texture*> m_colorTextures;
	Texture* m_depthTexture;
	Texture* m_stencilTexture;
};

#endif