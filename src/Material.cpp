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

PBRMaterial::PBRMaterial()
	:m_AO(nullptr),m_EmissionMap(nullptr), m_HeightMap(nullptr),m_AlbedoMap(nullptr),m_MetallicMap(nullptr), m_RoughnessMap(nullptr),m_NormalMap(nullptr)
{
}

PBRMaterial::PBRMaterial(Texture* albedo, Texture* normal, Texture* roughness, Texture* metallic, Texture* ao, Texture* emission, Texture* height)
	:m_AO(ao), m_EmissionMap(emission), m_HeightMap(height), m_AlbedoMap(albedo), m_MetallicMap(metallic), m_RoughnessMap(roughness), m_NormalMap(normal)
{
}

void PBRMaterial::Bind(Shader& shader) const
{
	shader.Bind();
	if (m_AlbedoMap) { m_AlbedoMap->Bind(0);		shader.setUniform1i("hasAlbedoMap", 1); }
	else 				shader.setUniform1i("hasAlbedoMap", 0);

	if(m_NormalMap)		{ m_NormalMap->Bind(1);		shader.setUniform1i("hasNormalMap", 1);}
	else 				shader.setUniform1i("hasNormalMap", 0);

	if(m_MetallicMap)   { m_MetallicMap->Bind(2);	shader.setUniform1i("hasMetallicMap", 1);}
	else 				shader.setUniform1i("hasMetallicMap", 0);

	if(m_RoughnessMap)	{ m_RoughnessMap->Bind(3); 	shader.setUniform1i("hasRoughnessMap", 1);}
	else 				shader.setUniform1i("hasRoughnessMap", 0);

	if(m_AO)			{ m_AO->Bind(4);			shader.setUniform1i("hasAO", 1);}
	else 				shader.setUniform1i("hasAO", 0);

	if(m_EmissionMap)	{ m_EmissionMap->Bind(5);	shader.setUniform1i("hasEmissionMap", 1);}
	else 				shader.setUniform1i("hasEmissionMap", 0);

	if(m_HeightMap)		{ m_HeightMap->Bind(6);		shader.setUniform1i("hasHeightMap", 1);}
	else 				shader.setUniform1i("hasHeightMap", 0);
}

void PBRMaterial::Unbind(Shader& shader) const
{
	shader.Bind();
	if (m_AlbedoMap)		m_AlbedoMap->Unbind();
	if (m_NormalMap)		m_NormalMap->Unbind();
	if (m_MetallicMap)   m_MetallicMap->Unbind();
	if (m_RoughnessMap)	m_RoughnessMap->Unbind();
	if (m_AO)			m_AO->Unbind();
	if (m_EmissionMap)	m_EmissionMap->Unbind();
	if (m_HeightMap)		m_HeightMap->Unbind();
}

void PBRMaterial::SetAO(Texture* map)
{
	m_AO = map;
}

void PBRMaterial::SetRoughnessMap(Texture* map)
{
	m_RoughnessMap = map;
}

void PBRMaterial::SetMetallicMap(Texture* map)
{
	m_MetallicMap = map;
}

void PBRMaterial::SetAlbedoMap(Texture* map)
{
	m_AlbedoMap = map;
}

void PBRMaterial::SetNormalMap(Texture* map)
{
	m_NormalMap = map;
}

void PBRMaterial::SetEmisionMap(Texture* map)
{
	m_EmissionMap = map;
}

void PBRMaterial::SetHeightMap(Texture* map)
{
	m_HeightMap = map;
}
