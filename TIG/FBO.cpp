#include "FBO.h"


FBO::FBO(int width, int height)
{
	m_width = width;
	m_height = height;
	m_depthTexture = nullptr;
	m_stencilTexture = nullptr;

	glGenFramebuffers(1, &m_FboHandle);
}

FBO::~FBO()
{
	reset();

	glDeleteFramebuffers(1, &m_FboHandle);
}


void FBO::attachColorTexture(unsigned int quantity)
{
	bind();
	for (unsigned int i = 0; i < quantity; i++) {
		auto tex = new Texture(m_width, m_height, TextureType::RGB);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + m_colorTextures.size(), GL_TEXTURE_2D, tex->getId(), 0);
		m_colorTextures.push_back(tex);
	}
	unbind();
}

void FBO::attachColorTexture(const GLchar* path, bool flip)
{
	bind();
	auto tex = new Texture(path, flip);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + m_colorTextures.size(), GL_TEXTURE_2D, tex->getId(), 0);
	m_colorTextures.push_back(tex);
	unbind();
}

void FBO::attachDepthTexture()
{
	bind();
	if (m_depthTexture != nullptr) {
		delete m_depthTexture;
	}
	m_depthTexture = new Texture(m_width, m_height, TextureType::Depth);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture->getId(), 0);
	unbind();
}

void FBO::attachDepthTexture(const GLchar* path, bool flip)
{
	bind();
	if (m_depthTexture != nullptr) {
		delete m_depthTexture;
	}
	m_depthTexture = new Texture(path, flip);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture->getId(), 0);
	unbind();
}


void FBO::attachStencilTexture()
{
	bind();
	if (m_stencilTexture != nullptr) {
		delete m_stencilTexture;
	}
	m_stencilTexture = new Texture(m_width, m_height, TextureType::Stencil);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_stencilTexture->getId(), 0);
	unbind();
}

void FBO::removeColorTextures()
{
	for (unsigned int i = 0; i < m_colorTextures.size(); i++) {
		delete m_colorTextures[i];
	}
	m_colorTextures.clear();
}

void FBO::removeDepthTexture()
{
	if (m_depthTexture != nullptr) {
		delete m_depthTexture;
	}
	m_depthTexture = nullptr;
}


void FBO::removeStencilTexture()
{
	if (m_stencilTexture != nullptr) {
		delete m_stencilTexture;
	}
	m_stencilTexture = nullptr;
}

void FBO::reset()
{
	removeColorTextures();
	removeDepthTexture();
}

void FBO::resize(int width, int height)
{
	m_width = width;
	m_height = height;
}


void FBO::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FboHandle);
}

void FBO::unbind()
{

	// TODO: remove
	// Any errors while generating fbo ?
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR: Unable to create FBO." << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


GLuint FBO::getColorTexture(unsigned int pos)
{
	if (pos < m_colorTextures.size()) {
		return m_colorTextures[pos]->getId();
	}
	else {
		return -1;
	}
}

GLuint FBO::getDepthTexture()
{
	if (m_depthTexture != nullptr) {
		return m_depthTexture->getId();
	}
	else {
		return -1;
	}
}

GLuint FBO::getStencilTexture()
{
	if (m_stencilTexture != nullptr) {
		return m_stencilTexture->getId();
	}
	else {
		return -1;
	}
}
