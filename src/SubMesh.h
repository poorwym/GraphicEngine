#pragma once

#include <vector>
#include <memory>
#include "Material.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Renderer.h"
#include "FrameBuffer.h"
#include "Camera.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class Mesh {
public:
    Mesh(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, Material* material);
    ~Mesh();

    void Bind() const;
    void Unbind() const;
    void Render(const Shader& shader, const Camera& camera, const glm::mat4& model, const glm::mat4* lightSpaceMatrix = nullptr) const;

    inline std::unique_ptr<Material> GetMaterial() const { return m_Material; }

private:
    VertexArray* m_VAO;
    IndexBuffer* m_IndexBuffer;
    VertexBuffer* m_VertexBuffer;
    FrameBuffer* m_FrameBuffer;
    Material* m_Material;
    PBRMaterial* m_PBRMaterial;

    std::vector<Vertex> m_Vertices;              // 顶点数据
    std::vector<unsigned int> m_Indices;         // 索引数据
    glm::mat4 m_LightSpaceMatrix;
};