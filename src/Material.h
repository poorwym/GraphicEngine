#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Shader.h"
#include "Texture.h"

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
};
