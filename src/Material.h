#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Shader.h"
#include "Texture.h"
#include"TinyOBJLoader/tiny_obj_loader.h"

class Material {
private:
    glm::vec3 m_Albedo;    // 反照率（基础颜色）
    float m_Metallic;      // 金属度
    float m_Roughness;     // 粗糙度

    Texture* m_DiffuseMap;    // 漫反射纹理（基础颜色）对应原来的u_Texture
    Texture* m_NormalMap;     // 法线贴图
    Texture* m_SpecularMap;   // 镜面反射贴图
public:
    // 构造函数
    Material(Texture* diffuse = nullptr, Texture* normal = nullptr, Texture* specular = nullptr);

    // 绑定材质和纹理
    void Bind(Shader& shader) const;
    void Unbind(Shader& shader) const;

    // 设置其他材质属性
    void SetAlbedo(Shader& shader, const glm::vec3& color);
    void SetMetallic(Shader& shader, float value);
    void SetRoughness(Shader& shader, float value);
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
    Texture* m_AO;
    Texture* m_RoughnessMap;
    Texture* m_MetallicMap;
    Texture* m_AlbedoMap;
    Texture* m_NormalMap;
    Texture* m_EmissionMap;
    Texture* m_HeightMap;
    Texture* m_SpecularExponentTextureMap;
    Texture* m_DissolveTextureMap;
    Texture* m_BumpMap;
    Texture* m_AlphaMap;

    int AlbedoMapSlot;
    int NormalMapSlot;
    int MetallicMapSlot;
    int RoughnessMapSlot;
    int AOMapSlot;
    int EmissionMapSlot;
    int AlphaMapSlot;
public:
    PBRMaterial(const std::string& filePath, const tinyobj::material_t& m);
    ~PBRMaterial();
    PBRMaterial(Texture* albedo, Texture* normal, Texture* roughness, Texture* metallic, Texture* ao, Texture* emission, Texture* height);

    inline int GetAlbedoMapSlot() const { return AlbedoMapSlot; };
    inline int GetNormalMapSlot()  const { return NormalMapSlot; };
    inline int GetMetallicMapSlot()   const { return MetallicMapSlot;};
    inline int GetRoughnessMapSlot()  const {    return RoughnessMapSlot; };
    inline int GetAOMapSlot()    const { return AOMapSlot; };
    inline int GetEmissionMapSlot()   const { return EmissionMapSlot; };
    inline int GetAlphaMapSlot()      const {return AlphaMapSlot; };
};
