#include "Material.h"
#include <iostream>
#include <string>
#include"SceneManager.h"

extern SceneManager sceneManager;

static std::string safe_substr(const std::string& str, size_t start) {
	if (start < str.size()) {
		std::cerr << str.substr(start) << std::endl;
		return str.substr(start);
	}
	else {
		std::cerr << "Warning: Attempted to substr at position " << start << " but string size is " << str.size() << ". Returning empty string." << std::endl;
		return "";
	}
}

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
static glm::vec3 ConvertToVec3(const float* v)
{
	return glm::vec3(v[0], v[1], v[2]);
}

PBRMaterial::PBRMaterial(const std::string& filePath, const tinyobj::material_t& m)
	: m_AO(nullptr), m_EmissionMap(nullptr), m_HeightMap(nullptr),
	m_AlbedoMap(nullptr), m_MetallicMap(nullptr), m_RoughnessMap(nullptr),
	m_NormalMap(nullptr), m_DissolveTextureMap(nullptr), m_SpecularExponentTextureMap(nullptr),m_Ambient(glm::vec3(0.0f)),
	m_Emission(glm::vec3(0.0f)), m_Diffuse(glm::vec3(0.8f)),
	m_Specular(glm::vec3(0.1f)), m_Transmittance(glm::vec3(0.0f)),
	m_Shininess(16.0f), m_Ior(1.5f), m_Illum(2),
	AlbedoMapSlot(-1), NormalMapSlot(-1), EmissionMapSlot(-1),
	MetallicMapSlot(-1), RoughnessMapSlot(-1),
	AlphaMapSlot(-1),AOMapSlot(-1)
{
	std::cout << "Load Texture " << filePath << std::endl;
	if(m.emission) m_Emission = glm::vec3(m.emission[0], m.emission[1], m.emission[2]);
	if (m.diffuse) m_Diffuse = glm::vec3(m.diffuse[0], m.diffuse[1], m.diffuse[2]);
	if (m.specular) m_Specular = glm::vec3(m.specular[0], m.specular[1], m.specular[2]);
	if(m.illum) m_Illum = m.illum;
	m_Shininess = m.illum == 5 ? m.shininess : 16.0f;
	if(m.ior) m_Ior = m.ior;
	if(m.ambient) m_Ambient = glm::vec3(m.ambient[0], m.ambient[1], m.ambient[2]);
	if(m.transmittance) m_Transmittance = glm::vec3(m.transmittance[0], m.transmittance[1], m.transmittance[2]);
	if(m.dissolve) m_d = m.dissolve;

	if (m.diffuse_texname.size() > 0){//1
        std::string Path = filePath + m.diffuse_texname;
		AlbedoMapSlot = sceneManager.AddTexture(Path.c_str());
	}
	if (m.bump_texname.size() > 0){//2
		std::string Path = filePath + m.bump_texname;
		NormalMapSlot = sceneManager.AddTexture(Path.c_str());
	}
	if (m.emissive_texname.size()) {//3
		std::string Path = filePath + m.emissive_texname;
        EmissionMapSlot = sceneManager.AddTexture(Path.c_str());
	}
	if (m.roughness_texname.size()) {//5
		std::string Path = filePath + m.roughness_texname;
        RoughnessMapSlot = sceneManager.AddTexture(Path.c_str());
	}
	if (m.metallic_texname.size()) {//6
		std::string Path = filePath + m.metallic_texname;
        MetallicMapSlot = sceneManager.AddTexture(Path.c_str());
	}
	if (m.normal_texname.size()) {//7
		std::string Path = filePath + m.normal_texname;
        NormalMapSlot = sceneManager.AddTexture(Path.c_str());
	}
	if (m.specular_highlight_texname.size()) {//8
		m_SpecularExponentTextureMap = new Texture(filePath+m.specular_highlight_texname.c_str());
		std::cout << filePath + m.specular_highlight_texname.c_str() << std::endl;
	}
	if (m.alpha_texname.size()) { //15
		std::string Path = filePath + m.alpha_texname;
		AlphaMapSlot = sceneManager.AddTexture(Path.c_str());
	}
}
PBRMaterial::~PBRMaterial()
{
	delete m_AlbedoMap;
	delete m_NormalMap;
	delete m_SpecularExponentTextureMap;
	delete m_DissolveTextureMap;
	delete m_HeightMap;
	delete m_AO;
	delete m_EmissionMap;
	delete m_MetallicMap;
	delete m_RoughnessMap;
	delete m_AlphaMap;
	delete m_BumpMap;
}
PBRMaterial::PBRMaterial(Texture* albedo, Texture* normal, Texture* roughness, Texture* metallic, Texture* ao, Texture* emission, Texture* height)
	:m_AO(ao), m_EmissionMap(emission), m_HeightMap(height), m_AlbedoMap(albedo), m_MetallicMap(metallic), m_RoughnessMap(roughness), m_NormalMap(normal)
{
}
