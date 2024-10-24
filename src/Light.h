#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"

class Light {
public:
    glm::vec3 position;
    glm::vec3 color;
    float intensity;

    // 不同光源类型可以继承此类
    virtual void Apply(Shader& shader) const = 0;
};

class PointLight : public Light {
public:
    float range;
    void Apply(Shader& shader) const override;  // 将光照数据传递给着色器
};