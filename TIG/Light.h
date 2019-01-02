#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

class Light
{
public:
	Light(const glm::vec3 &pos, const glm::vec3 &diff, const glm::vec3 &spec, const glm::vec3 ambient);
	~Light();

	glm::vec3 getPosition();
	glm::vec3 getDiffuse();
	glm::vec3 getSpecular();
	glm::vec3 getAmbient();


private:
	glm::vec3 m_position;
	glm::vec3 m_diffuse;
	glm::vec3 m_specular;
	glm::vec3 m_ambient;
};
#endif