#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Material.h"
#include "Light.h"


class Shader
{
public:
	// constructor reads and builds the shader
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);

	// use/activate the shader
	void use();

	// update shader / uniform variables
	virtual void update() = 0;

	// get shader ID
	GLuint getId();

	void setMaterial(Material* material);
	void addLight(Light *light);
	void clearLights();

	// utility uniform functions
	void setBool(const std::string &name, bool value) const;
	void set1i(const std::string &name, int value) const;
	void set1f(const std::string &name, float value) const;
	void set2f(const std::string &name, glm::vec2 value) const;
	void set3f(const std::string &name, glm::vec3 value) const;
	void set4f(const std::string &name, glm::vec4 value) const;
	void setMat2f(const std::string &name, glm::mat2 value) const;
	void setMat3f(const std::string &name, glm::mat3 value) const;
	void setMat4f(const std::string &name, glm::mat4 value) const;

protected:
	unsigned int ID;
	Material* m_material;
	std::vector<Light*> m_lights;
	
	std::string readFile(const char *filePath);
};

#endif
