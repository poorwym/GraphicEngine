#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Shader.h"
#include "Texture.h"

class Material {
private:
    glm::vec3 albedo;    // �����ʣ�������ɫ��
    float metallic;      // ������
    float roughness;     // �ֲڶ�
public:
    // ����
    Texture* diffuseMap;    // ����������������ɫ��
    Texture* normalMap;     // ������ͼ
    Texture* specularMap;   // ���淴����ͼ

    // ���캯��
    Material(Texture* diffuse = nullptr, Texture* normal = nullptr, Texture* specular = nullptr);

    // �󶨲��ʺ�����
    void Bind(Shader& shader) const;

    // ����������������
    void SetAlbedo(Shader& shader, const glm::vec3& color);
    void SetMetallic(Shader& shader, float value);
    void SetRoughness(Shader& shader, float value);
};
