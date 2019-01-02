#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Material.h"
#include "Shader.h"
#include "Geometry.h"

class Mesh
{
public:
	Mesh(const std::string meshPath, Material* mat);
	~Mesh();

	void Draw(Shader* shader);

private:
	std::vector<Geometry*> m_geometry;
	Material* m_material;

	void loadModel(const std::string path);
	void processNode(aiNode *node, const aiScene *scene);
	Geometry* processGeometry(aiMesh *mesh, const aiScene *scene);
};
#endif
