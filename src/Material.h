#pragma once

#include "glm/glm.hpp"
#include "Texture.h"
//#include <fbxsdk.h>

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
    float Roughness;
    float Metallic;
    bool operator==(const Material& other) const
    {
        return AlbedoMapIndex == other.AlbedoMapIndex && NormalMapIndex == other.NormalMapIndex &&
            MetallicMapIndex == other.MetallicMapIndex && RoughnessMapIndex == other.RoughnessMapIndex &&
            AOMapIndex == other.AOMapIndex && EmissionMapIndex == other.EmissionMapIndex &&
            AlphaMapIndex == other.AlphaMapIndex && HeightMapIndex == other.HeightMapIndex &&
            Ambient == other.Ambient && Diffuse == other.Diffuse && Specular == other.Specular &&
            Emission == other.Emission && Dissolve == other.Dissolve && Illum== other.Illum && OpticalDensity == other.OpticalDensity &&
            SpecularExponent == other.SpecularExponent && HeightMap == other.HeightMap && Roughness == other.Roughness && Metallic == other.Metallic;
    }
};

class PBRMaterial{
private:
    Material m_Material; 
    std::string m_Name;
public:
    PBRMaterial();
    PBRMaterial(const std::string& name);
    PBRMaterial(Material& m, const std::string& name) { m_Material = m; m_Name = name; };
    ~PBRMaterial();

    void SetMaterial(const Material& material);
    inline const std::string& GetName() const { return m_Name; };
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

const Material default_material = {
        -1.0f, // AlbedoMapIndex
        -1.0f, // NormalMapIndex
        -1.0f, // MetallicMapIndex
        -1.0f, // RoughnessMapIndex
        -1.0f, // AOMapIndex
        -1.0f, // EmissionMapIndex
        -1.0f, // AlphaMapIndex
        -1.0f, // HeightMapIndex
        // Material properties
        glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), // Ambient
        glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), // Diffuse
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), // Specular
        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), // Emission
        1.0f, // Dissolve
        2.0f, // Illum
        1.0f, // OpticalDensity
        0.0f, // SpecularExponent
        0.0f, // BumpMutiplier
        0,    // HeightMap
        0.8f, // Roughness
        0.0f  // Metallic
};