#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Shader.h"
#include "Texture.h"
#include"TinyOBJLoader/tiny_obj_loader.h"


struct Material {
    float AlbedoMapIndex; //0
    float NormalMapIndex; //1
    float MetallicMapIndex; //2
    float RoughnessMapIndex; //3
    float AOMapIndex; //4
    float EmissionMapIndex; //5
    float AlphaMapIndex; //6
    float padding;
};

class PBRMaterial{
private:
    glm::vec3   m_Ambient;
    glm::vec3   m_Diffuse;
    glm::vec3   m_Specular;
    glm::vec3   m_Transmittance;
    glm::vec3   m_Emission;
    float       m_Shininess;
    float       m_Ior;
    float       m_d;
    int         m_Illum;

    Material m_Material; 
public:
    PBRMaterial(const std::string& filePath, const tinyobj::material_t& m);
    ~PBRMaterial();
    inline float GetAlbedoMapSlot() const { return m_Material.AlbedoMapIndex; };
    inline float GetNormalMapSlot()  const { return m_Material.NormalMapIndex; };
    inline float GetMetallicMapSlot()   const { return m_Material.MetallicMapIndex;};
    inline float GetRoughnessMapSlot()  const {    return m_Material.RoughnessMapIndex; };
    inline float GetAOMapSlot()    const { return m_Material.AOMapIndex; };
    inline float GetEmissionMapSlot()   const { return m_Material.EmissionMapIndex; };
    inline float GetAlphaMapSlot()      const {return m_Material.AlphaMapIndex; };
};
