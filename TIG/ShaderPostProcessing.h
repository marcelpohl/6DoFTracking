#ifndef SHADERPOSTPROCESSING_H
#define SHADERPOSTPROCESSING_H

#include <glad/glad.h>
#include <vector>
#include <string>
#include "Shader.h"
#include "Texture.h"

class ShaderPostProcessing : public Shader
{
public:
	ShaderPostProcessing(const GLchar* vertexPath, const GLchar* fragmentPath);
	~ShaderPostProcessing();

	void addTexture(GLuint texture, std::string uniformName);

	void update();
	void draw();

private:
	std::vector<float> m_quadVertices;
	GLuint m_quadVAO, m_quadVBO;
	std::vector<GLuint> m_textures;
	std::vector<std::string> m_uniformTexNames;
};

#endif