#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

class Geometry
{
public:
	Geometry(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
	~Geometry();

	std::vector<Vertex>* getVertices();
	std::vector<GLuint>* getIndices();

	void Draw();

private:
	GLuint m_VAO, m_VBO, m_EBO;

	std::vector<Vertex> m_vertices;
	std::vector<GLuint> m_indices;

	void setup();
};

#endif