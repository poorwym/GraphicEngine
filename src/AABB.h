#pragma once
#include "glm/glm.hpp"

struct AABB {
    glm::vec3 min;
    float pad1;
    glm::vec3 max;
    float pad2;
};