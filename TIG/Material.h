#ifndef MATERIAL_H
#define MATERIAL_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Texture.h"

enum class MapType {
	Diffuse,
	Specular,
	AmbientOcclusion
};

class Material
{
private:
	glm::vec3 m_ambientCol;
	glm::vec3 m_diffuseCol;
	glm::vec3 m_specularCol;
	float m_shininess;

	bool m_hasDiffuseTexture;
	bool m_hasSpecularTexture;
	bool m_hasAOTexture;

	Texture* m_diffuseTexture;
	Texture* m_specularTexture;
	Texture* m_ambientOcclusionTexture;

	void initialize(const GLchar* diffTexPath, const GLchar* specTexPath, const GLchar* ambOccTexPath, float shiny);

public:
	Material(glm::vec3 diffuse, glm::vec3 ambient, glm::vec3 spec, float shiny);
	Material(const GLchar* diffTexPath, float shiny);
	Material(const GLchar* diffTexPath, const GLchar* specTexPath, const GLchar* ambOccTexPath, float shiny);
	~Material();

	void setAmbientColor(const glm::vec3 &color);
	void setDiffuseColor(const glm::vec3 &color);
	void setSpecularColor(const glm::vec3 &color);
	void setShininess(float val);

	glm::vec3 getDiffuseColor();
	glm::vec3 getAmbientColor();
	glm::vec3 getSpecularColor();
	float getShininess();

	void setTexture(const GLchar* path, MapType type);
	void deleteTexture(MapType type);

	bool hasTexture(MapType type);
	Texture* getTexture(MapType type);

};

#endif