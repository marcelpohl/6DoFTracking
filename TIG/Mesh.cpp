#include "Mesh.h"

Mesh::Mesh(const std::string meshPath, Material* mat)
{
	m_material = mat;
	loadModel(meshPath);
}

Mesh::~Mesh()
{
	for (unsigned int i = 0; i < m_geometry.size(); i++) {
		delete m_geometry[i];
	}
	m_geometry.clear();
}


void Mesh::Draw(Shader* shader)
{
	shader->use();
	shader->setMaterial(m_material);
	shader->update();

	for (unsigned int i = 0; i < m_geometry.size(); i++) {
		m_geometry[i]->Draw();
	}
}


void Mesh::loadModel(const std::string path)
{
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	//directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

void Mesh::processNode(aiNode *node, const aiScene *scene)
{
	// process node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		m_geometry.push_back(processGeometry(mesh, scene));
	}

	// then its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Geometry* Mesh::processGeometry(aiMesh *mesh, const aiScene *scene)
{
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	// process vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;

		// position
		glm::vec3 vec;
		vec.x = mesh->mVertices[i].x;
		vec.y = mesh->mVertices[i].y;
		vec.z = mesh->mVertices[i].z;
		vertex.Position = vec;

		// normal
		vec.x = mesh->mNormals[i].x;
		vec.y = mesh->mNormals[i].y;
		vec.z = mesh->mNormals[i].z;
		vertex.Normal = vec;

		// texture coordinates
		if (mesh->mTextureCoords[0])
		{
			glm::vec2 texCoord;
			texCoord.x = mesh->mTextureCoords[0][i].x;
			texCoord.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = texCoord;
		}
		else
		{
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}

		vertices.push_back(vertex);
	}

	// process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	return new Geometry(vertices, indices);
}
