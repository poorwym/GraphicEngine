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
    float HeightMapIndex; //7
    //Ka
    glm::vec4 Ambient;
    //Kd
    glm::vec4 Diffuse;
    //Ks
    glm::vec4 Specular;
    //Ke
    glm::vec4 Emission;
    //d
    float Dissolve;
    //illum
    float Illum;
    //Ni
    float OpticalDensity;
    //Ns
    float SpecularExponent;
    float padding1;
    float padding2;
    float padding3;
    float padding4;
};

class PBRMaterial{
private:
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
