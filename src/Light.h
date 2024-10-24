#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"

class Light {
public:
    glm::vec3 position;
    glm::vec3 color;
    float intensity;

    // ��ͬ��Դ���Ϳ��Լ̳д���
    virtual void Apply(Shader& shader) const = 0;
};

class PointLight : public Light {
public:
    float range;
    void Apply(Shader& shader) const override;  // ���������ݴ��ݸ���ɫ��
};