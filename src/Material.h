#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Shader.h"

class Material {
public:
    glm::vec3 albedo;
    float metallic;
    float roughness;

    void Apply(Shader& shader) const; // ���������Դ��ݸ���ɫ��
};