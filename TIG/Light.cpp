#include "Light.h"



Light::Light(const glm::vec3 &pos, const glm::vec3 &diff, const glm::vec3 &spec, const glm::vec3 ambient)
{
	m_position = pos;
	m_diffuse = diff;
	m_specular = spec;
	m_ambient = ambient;
}


Light::~Light()
{
}


glm::vec3 Light::getPosition()
{
	return m_position;
}

glm::vec3 Light::getDiffuse()
{
	return m_diffuse;
}

glm::vec3 Light::getSpecular()
{
	return m_specular;
}

glm::vec3 Light::getAmbient()
{
	return m_ambient;
}