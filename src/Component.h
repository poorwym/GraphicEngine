#pragma once
#include "Mesh.h"
#include "Material.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Component {
public:
    virtual ~Component() = default;
    virtual void Update(float deltaTime) = 0;     // 更新组件
};

class TransformComponent : public Component {
public:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    TransformComponent();
    void Update(float deltaTime) override;
};

class RenderComponent : public Component {
public:
    Mesh* mesh;
    Material* material;

    RenderComponent(Mesh* mesh, Material* material);
    void Update(float deltaTime) override;
};