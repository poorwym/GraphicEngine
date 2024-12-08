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
    //bump
    float BumpMutiplier;
    uint32_t HeightMap; //CPU�˵ĸ߶�ͼ
    float padding3;
    float padding4;
    bool operator==(const Material& other) const
    {
        return AlbedoMapIndex == other.AlbedoMapIndex && NormalMapIndex == other.NormalMapIndex &&
            MetallicMapIndex == other.MetallicMapIndex && RoughnessMapIndex == other.RoughnessMapIndex &&
            AOMapIndex == other.AOMapIndex && EmissionMapIndex == other.EmissionMapIndex &&
            AlphaMapIndex == other.AlphaMapIndex && HeightMapIndex == other.HeightMapIndex &&
            Ambient == other.Ambient && Diffuse == other.Diffuse && Specular == other.Specular &&
            Emission == other.Emission && Dissolve == other.Dissolve && Illum== other.Illum && OpticalDensity == other.OpticalDensity &&
            SpecularExponent == other.SpecularExponent && HeightMap == other.HeightMap;
    }
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
    inline float GetHeightMapSlot()    const { return m_Material.HeightMapIndex; };
    inline const Material& GetMaterial() const { return m_Material; }
};
