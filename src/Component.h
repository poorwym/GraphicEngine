#pragma once
#include "Mesh.h"
#include "Material.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Component {
private:
    Mesh* mesh;
    Material* material;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    Entity* fatherEntity;

public:

    TransformComponent();
    RenderComponent(Mesh* mesh, Material* material);
    void Update(float deltaTime) override;
};

