#pragma once
#include "Mesh.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>


class Component {
protected:
    std::string m_Type;
    glm::mat4 m_LocalTransform;
public:
    Component();
    virtual void SetType() {};
    virtual void Render(Shader& shader, Camera& camera, glm::mat4 globalTranform) {};
    inline std::string GetType() { return m_Type;}
    virtual void Translate(glm::vec3& translation) {};
    virtual void Update(float deltaTime) {};
};

class MeshComponent : public Component
{
private:
    Mesh* m_Mesh;
public:
    MeshComponent(Mesh* mesh);
    void Render(Shader& shader, Camera& camera, glm::mat4 globalTranform);
    inline void SetType() override { m_Type = "MeshComponent";  } 
    void Update(float deltaTime) override;
};

