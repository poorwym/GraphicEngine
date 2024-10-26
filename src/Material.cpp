#include "Material.h"

Material::Material(Texture* diffuse, Texture* normal, Texture* specular)
	: m_DiffuseMap(nullptr), m_NormalMap(nullptr), m_SpecularMap(nullptr),
	m_Albedo(glm::vec3(1.0f)), m_Metallic(0.5f), m_Roughness(1.0f)
{
	m_DiffuseMap = diffuse;
	m_NormalMap = normal;
	m_SpecularMap = specular;

}

void Material::Bind(Shader& shader) const
{
	shader.Bind();
	shader.setUniformVec3f("albedo", m_Albedo);
	shader.setUniform1f("roughness", m_Roughness);
	shader.setUniform1f("metallic", m_Metallic);
	if (m_DiffuseMap) {
		m_DiffuseMap->Bind(0);
	}
	if (m_NormalMap) {
		m_NormalMap->Bind(1);
	}
	if (m_SpecularMap) {
		m_SpecularMap->Bind(2);
	}
}

void Material::Unbind(Shader& shader) const
{
	if (m_DiffuseMap) {
		m_DiffuseMap->Unbind();
	}
	if (m_NormalMap) {
		m_NormalMap->Unbind();
	}
	if (m_SpecularMap) {
		m_SpecularMap->Unbind();
	}
}

void Material::SetAlbedo(Shader& shader, const glm::vec3& color)
{
	m_Albedo = color;
	shader.setUniformVec3f("u_Material.Albedo", m_Albedo);
}
void Material::SetMetallic(Shader& shader, float value)
{
	m_Metallic = value;
	shader.setUniform1f("u_Material.Metallic", m_Metallic);
}
void Material::SetRoughness(Shader& shader, float value)
{
	m_Roughness = value;
	 shader.setUniform1f("u_Material.Roughness", m_Roughness);
}
