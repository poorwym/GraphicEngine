#pragma once
#include "Mesh.h"
#include <glm/glm.hpp>
#include <string>
#include "ShaderStorageBuffer.h"

class Component {
protected:
    std::string m_Type;
    glm::mat4 m_LocalTransform;
public:
    Component();
    virtual void SetType() {};
    virtual void RenderDepthMap(Shader& shader, glm::mat4 globalTransform) {};
    virtual void Render(Shader& shader, Camera& camera, glm::mat4 globalTranform) {};
    inline std::string GetType() { return m_Type;}
    virtual void Translate(glm::vec3& translation) {};
    virtual void Update(float deltaTime) {};
};

class MeshComponent : public Component
{
private:
    int offset;
    int m_NumVertices;
    std::vector<Mesh*> m_Meshes;
    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    std::vector<Vertex> m_TransformedVertices;
    ShaderStorageBuffer m_InputSSBO;
    ShaderStorageBuffer m_OutputSSBO;
public:
    MeshComponent();
    ~MeshComponent();
    void AddMesh(Mesh* mesh);
    void RenderDepthMap(Shader& shader, glm::mat4 globalTransform) override;
    void Render(Shader& shader, Camera& camera, glm::mat4 globalTranform) override;
    inline void SetType() override { m_Type = "MeshComponent";  } 
    void Update(float deltaTime) override;

    std::vector<Vertex>* GetVertices(const glm::mat4& globalTransform);
    std::vector<unsigned int>* GetIndices();
};

