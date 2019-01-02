#include "Phong.h"

Phong::Phong(const GLchar* vertexPath, const GLchar* fragmentPath) : Shader(vertexPath, fragmentPath)
{
}


Phong::~Phong()
{
}

void Phong::update()
{
	if (m_material != nullptr) {
		set3f("mat.ambient", m_material->getAmbientColor());
		set3f("mat.diffuse", m_material->getDiffuseColor());
		set3f("mat.specular", m_material->getSpecularColor());
		set1f("mat.shininess", m_material->getShininess());

		setBool("mat.hasDiffuseTex", m_material->hasTexture(MapType::Diffuse));
		if (m_material->hasTexture(MapType::Diffuse)) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_material->getTexture(MapType::Diffuse)->getId());
			set1i("mat.diffuseTex", 0);
		}

		setBool("mat.hasSpecularTex", m_material->hasTexture(MapType::Specular));
		if (m_material->hasTexture(MapType::Specular)) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_material->getTexture(MapType::Specular)->getId());
			set1i("mat.specularTex", 1);
		}

		setBool("mat.hasAmbientOocclusionTex", m_material->hasTexture(MapType::AmbientOcclusion));
		if (m_material->hasTexture(MapType::AmbientOcclusion)) {
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, m_material->getTexture(MapType::AmbientOcclusion)->getId());
			set1i("mat.ambientOcclusionTex", 2);
		}
	}

	set1i("numLights", m_lights.size());
	for (unsigned int i = 0; i < m_lights.size(); i++) {
		std::string index = std::to_string(i);
		set3f("lights[" + index + "].position", m_lights[i]->getPosition());
		set3f("lights[" + index + "].diffuse", m_lights[i]->getDiffuse());
		set3f("lights[" + index + "].specular", m_lights[i]->getSpecular());
		set3f("lights[" + index + "].ambient", m_lights[i]->getAmbient());
	}
}

