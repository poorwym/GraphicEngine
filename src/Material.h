#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Shader.h"
#include "Texture.h"
#include"TinyOBJLoader/tiny_obj_loader.h"

class Material {
private:
    glm::vec3 m_Albedo;    // �����ʣ�������ɫ��
    float m_Metallic;      // ������
    float m_Roughness;     // �ֲڶ�

    Texture* m_DiffuseMap;    // ����������������ɫ����Ӧԭ����u_Texture
    Texture* m_NormalMap;     // ������ͼ
    Texture* m_SpecularMap;   // ���淴����ͼ
public:
    // ���캯��
    Material(Texture* diffuse = nullptr, Texture* normal = nullptr, Texture* specular = nullptr);

    // �󶨲��ʺ�����
    void Bind(Shader& shader) const;
    void Unbind(Shader& shader) const;

    // ����������������
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
public:
    PBRMaterial();
    PBRMaterial(const std::string& filePath, const tinyobj::material_t& m);
    ~PBRMaterial();
    PBRMaterial(Texture* albedo, Texture* normal, Texture* roughness, Texture* metallic, Texture* ao, Texture* emission, Texture* height);
    // �󶨲��ʺ�����
    void Bind(Shader& shader) const;
    void Unbind(Shader& shader) const;

    void SetAO(Texture* map);
    void SetRoughnessMap(Texture* map);
    void SetMetallicMap(Texture* map);
    void SetAlbedoMap(Texture* map);
    void SetNormalMap(Texture* map);
    void SetEmisionMap(Texture* map);
    void SetHeightMap(Texture* map);
};
