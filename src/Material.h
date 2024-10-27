#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Shader.h"
#include "Texture.h"

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
};
