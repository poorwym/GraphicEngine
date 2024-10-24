#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Shader.h"
#include "Texture.h"

class Material {
private:
    glm::vec3 albedo;    // 反照率（基础颜色）
    float metallic;      // 金属度
    float roughness;     // 粗糙度
public:
    // 纹理
    Texture* diffuseMap;    // 漫反射纹理（基础颜色）
    Texture* normalMap;     // 法线贴图
    Texture* specularMap;   // 镜面反射贴图

    // 构造函数
    Material(Texture* diffuse = nullptr, Texture* normal = nullptr, Texture* specular = nullptr);

    // 绑定材质和纹理
    void Bind(Shader& shader) const;

    // 设置其他材质属性
    void SetAlbedo(Shader& shader, const glm::vec3& color);
    void SetMetallic(Shader& shader, float value);
    void SetRoughness(Shader& shader, float value);
};
