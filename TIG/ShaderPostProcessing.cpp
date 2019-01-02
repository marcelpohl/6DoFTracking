#include "ShaderPostProcessing.h"

ShaderPostProcessing::ShaderPostProcessing(const GLchar* vertexPath, const GLchar* fragmentPath) : Shader(vertexPath, fragmentPath)
{
	m_quadVertices = {
		// positions	// texCoords
		-1.0f, 1.0f,	0.0f, 1.0f,
		-1.0f, -1.0f,	0.0f, 0.0f,
		1.0f, -1.0f,	1.0f, 0.0f,

		-1.0f, 1.0f,	0.0f, 1.0f,
		1.0f, -1.0f,	1.0f, 0.0f,
		1.0f, 1.0f,		1.0f, 1.0f
	};

	glGenVertexArrays(1, &m_quadVAO);
	glGenBuffers(1, &m_quadVBO);
	glBindVertexArray(m_quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_quadVertices.size(), &m_quadVertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);
}


ShaderPostProcessing::~ShaderPostProcessing()
{
}

void ShaderPostProcessing::addTexture(GLuint texture, std::string uniformName)
{
	m_textures.push_back(texture);
	m_uniformTexNames.push_back(uniformName);
}

void ShaderPostProcessing::update()
{
	for (unsigned int i = 0; i < m_textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textures[i]);
		set1i(m_uniformTexNames[i], i);
	}
}

void ShaderPostProcessing::draw()
{
	glBindVertexArray(m_quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}
