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

static glm::vec3 ConvertToVec3(const float* v)
{
	return glm::vec3(v[0], v[1], v[2]);
}

PBRMaterial::PBRMaterial(const std::string& filePath, const tinyobj::material_t& m)
	:m_Emission(glm::vec3(0.0f)), m_Diffuse(glm::vec3(0.8f)),
	m_Specular(glm::vec3(0.1f)), m_Transmittance(glm::vec3(0.0f)),
	m_Shininess(16.0f), m_Ior(1.5f), m_Illum(2),
	m_Material({-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f})
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
		m_Material.AlbedoMapIndex = sceneManager.AddTexture(Path.c_str());
	}
	if (m.bump_texname.size() > 0){//2
		std::string Path = filePath + m.bump_texname;
        m_Material.NormalMapIndex = sceneManager.AddTexture(Path.c_str());
	}
	if (m.emissive_texname.size()) {//3
		std::string Path = filePath + m.emissive_texname;
        m_Material.EmissionMapIndex = sceneManager.AddTexture(Path.c_str());
	}
	if (m.roughness_texname.size()) {//5
		std::string Path = filePath + m.roughness_texname;
        m_Material.RoughnessMapIndex = sceneManager.AddTexture(Path.c_str());
	}
	if (m.metallic_texname.size()) {//6
		std::string Path = filePath + m.metallic_texname;
        m_Material.MetallicMapIndex = sceneManager.AddTexture(Path.c_str());
	}
	if (m.normal_texname.size()) {//7
		std::string Path = filePath + m.normal_texname;
        m_Material.NormalMapIndex = sceneManager.AddTexture(Path.c_str());
	}
	if (m.specular_highlight_texname.size()) {//8
		//m_SpecularExponentTextureMap = new Texture(filePath+m.specular_highlight_texname.c_str());
		std::cout << filePath + m.specular_highlight_texname.c_str() << std::endl;
	}
	if (m.alpha_texname.size()) { //15
		std::string Path = filePath + m.alpha_texname;
        m_Material.AlphaMapIndex = sceneManager.AddTexture(Path.c_str());
	}
}
PBRMaterial::~PBRMaterial()
{
	
}
