#include "Material.h"


Material::Material(glm::vec3 diffuse, glm::vec3 ambient, glm::vec3 spec, float shiny)
{
	m_diffuseCol = diffuse;
	m_ambientCol = ambient;
	m_specularCol = spec;
	m_shininess = shiny;

	m_hasDiffuseTexture = false;
	m_hasSpecularTexture = false;
	m_hasAOTexture = false;

	m_diffuseTexture = nullptr;
	m_specularTexture = nullptr;
	m_ambientOcclusionTexture = nullptr;
}

Material::Material(const GLchar* diffTexPath, float shiny)
{
	initialize(diffTexPath, nullptr, nullptr, shiny);
}

Material::Material(const GLchar* diffTexPath, const GLchar* specTexPath, const GLchar* ambOccTexPath, float shiny)
{
	initialize(diffTexPath, specTexPath, ambOccTexPath, shiny);
}


Material::~Material()
{
	if (m_hasDiffuseTexture) {
		delete m_diffuseTexture;
		m_diffuseTexture = nullptr;
		m_hasDiffuseTexture = false;
	}

	if (m_hasSpecularTexture) {
		delete m_specularTexture;
		m_specularTexture = nullptr;
		m_hasSpecularTexture = false;
	}

	if (m_hasAOTexture) {
		delete m_ambientOcclusionTexture;
		m_ambientOcclusionTexture = nullptr;
		m_hasAOTexture = false;
	}
}


void Material::initialize(const GLchar* diffTexPath, const GLchar* specTexPath, const GLchar* ambOccTexPath, float shiny)
{
	if (diffTexPath != nullptr) {
		m_diffuseTexture = new Texture(diffTexPath);
		m_hasDiffuseTexture = true;
	}

	if (specTexPath != nullptr) {
		m_specularTexture = new Texture(specTexPath);
		m_hasSpecularTexture = true;
	}

	if (ambOccTexPath != nullptr) {
		m_ambientOcclusionTexture = new Texture(ambOccTexPath);
		m_hasAOTexture = true;
	}
}


void Material::setDiffuseColor(const glm::vec3 &color)
{
	m_diffuseCol = color;
}

void Material::setAmbientColor(const glm::vec3 &color)
{
	m_ambientCol = color;
}

void Material::setSpecularColor(const glm::vec3 &color)
{
	m_specularCol = color;
}

void Material::setShininess(float val)
{
	m_shininess = val;
}


glm::vec3 Material::getDiffuseColor()
{
	return m_diffuseCol;
}

glm::vec3 Material::getAmbientColor()
{
	return m_ambientCol;
}

glm::vec3 Material::getSpecularColor()
{
	return m_specularCol;
}

float Material::getShininess()
{
	return m_shininess;
}


void Material::setTexture(const GLchar* path, MapType type)
{
	if (path != nullptr) {
		switch (type) {
		case MapType::Diffuse:
			if (m_hasDiffuseTexture) {
				delete m_diffuseTexture;
			}
			m_diffuseTexture = new Texture(path);
			m_hasDiffuseTexture = true;
			break;
		case MapType::Specular:
			if (m_hasSpecularTexture) {
				delete m_specularTexture;
			}
			m_specularTexture = new Texture(path);
			m_hasSpecularTexture = true;
			break;
		case MapType::AmbientOcclusion:
			if (m_hasAOTexture) {
				delete m_ambientOcclusionTexture;
			}
			m_ambientOcclusionTexture = new Texture(path);
			m_hasAOTexture = true;
			break;
		}
	}
}

void Material::deleteTexture(MapType type)
{
	switch (type) {
	case MapType::Diffuse:
		if (m_hasDiffuseTexture) {
			delete m_diffuseTexture;
		}
		m_hasDiffuseTexture = false;
		break;
	case MapType::Specular:
		if (m_hasSpecularTexture) {
			delete m_specularTexture;
		}
		m_hasSpecularTexture = false;
		break;
	case MapType::AmbientOcclusion:
		if (m_hasAOTexture) {
			delete m_ambientOcclusionTexture;
		}
		m_hasAOTexture = false;
		break;
	}
}


bool Material::hasTexture(MapType type)
{
	switch (type) {
	case MapType::Diffuse:
		return m_hasDiffuseTexture;
	case MapType::Specular:
		return m_hasSpecularTexture;
	case MapType::AmbientOcclusion:
		return m_hasAOTexture;
	}

	return false;
}

Texture* Material::getTexture(MapType type)
{
	switch (type) {
	case MapType::Diffuse:
		return m_diffuseTexture;
	case MapType::Specular:
		return m_specularTexture;
	case MapType::AmbientOcclusion:
		return m_ambientOcclusionTexture;
	}

	return nullptr;
}