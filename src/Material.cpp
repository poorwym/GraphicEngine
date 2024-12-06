#include "Material.h"
#include <iostream>
#include <string>
#include"SceneManager.h"
#include "TextureManager.h"

extern TextureManager textureManager;

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
	:m_Material({
	    -1.0f,
	    -1.0f,
	    -1.0f,
	    -1.0f,
	    -1.0f,
	    -1.0f,
	    -1.0f,
	    -1.0f,
		// Material properties
		glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), // Default ambient color (Ka)
		glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), // Default diffuse color (Kd)
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), // Default specular color (Ks)
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), // Default emission color (Ke)
		1.0f,                               // Default transparency (d)
		2.0f,                               // Default illumination model
		1.0f,								// Default optical density (Ni)
		0.0f,								// Default shininess (Ns)
		
		0.0f,
		0,
		0.0f,
		0.0f
	})
{
	std::cout << "Load Texture " << filePath << std::endl;
	if (m.ambient) m_Material.Ambient = glm::vec4(m.ambient[0], m.ambient[1], m.ambient[2], 0.0f);
	if (m.diffuse) m_Material.Diffuse = glm::vec4(m.diffuse[0], m.diffuse[1], m.diffuse[2], 0.0f);
	if (m.specular) m_Material.Specular = glm::vec4(m.specular[0], m.specular[1], m.specular[2], 0.0f);
	if (m.illum) m_Material.Illum = m.illum;
	if (m.dissolve) m_Material.Dissolve = m.dissolve;
	if (m.emission) m_Material.Emission = glm::vec4(m.emission[0], m.emission[1], m.emission[2], 0.0f);
	if (m.shininess) m_Material.SpecularExponent = m.shininess;
	if (m.ior) m_Material.OpticalDensity = m.ior;



	if (m.diffuse_texname.size() > 0){//1
        std::string Path = filePath + m.diffuse_texname;
		m_Material.AlbedoMapIndex = sceneManager.AddTexture(Path.c_str());
	}
	if (m.bump_texname.size() > 0){//2
		std::string Path = filePath + m.bump_texname;
        m_Material.HeightMapIndex = sceneManager.AddTexture(Path.c_str());
		m_Material.BumpMutiplier = 0.05;
		m_Material.HeightMap = textureManager.AddTexture(new CPUTexture(Path));
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
	if (m.alpha_texname.size()) { //15
		std::string Path = filePath + m.alpha_texname;
        m_Material.AlphaMapIndex = sceneManager.AddTexture(Path.c_str());
	}
}
PBRMaterial::~PBRMaterial()
{
	
}
