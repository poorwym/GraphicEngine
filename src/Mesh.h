#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <memory>
#include "Texture.h"
#include "Material.h"
#include "FrameBuffer.h"
#include<Camera.h>

// 顶点结构体，包含位置信息、法线、UV 等
struct Vertex {
    glm::vec4 Position;
    glm::vec4 Normal;
    glm::vec4 TexCoords;
    glm::vec4 Tangent;
    glm::vec4 Bitangent;
    float MaterialIndex;
    float padding1;
    float padding2;
    float padding3;
};

struct Triangle {
    glm::vec4 position[3];
    glm::vec4 normal[3];
    glm::vec4 texCoords[3];
    glm::vec4 tangent[3];
    glm::vec4 bitangent[3];
    int materialIndex;
    float padding1;
    float padding2;
    float padding3;
};

class Mesh {
public:
    Mesh(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, Material* material);
    Mesh(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, PBRMaterial* PBRmaterial, float scaleRate);
    ~Mesh();

    void Bind() const;                         // 绑定 VAO，准备绘制
    void Unbind() const;                       // 解除 VAO 绑定

    inline const std::vector<Vertex>& GetVertices() const { return m_Vertices; };
    inline const std::vector<unsigned int>& GetIndices() const { return m_Indices; };
    inline const IndexBuffer* GetIndexBuffer() const { return m_IndexBuffer; };


private:
    VertexArray* m_VAO;
    IndexBuffer* m_IndexBuffer;
    VertexBuffer* m_VertexBuffer;
    Material* m_Material;
    PBRMaterial*  m_PBRMaterial;

    std::vector<Vertex> m_Vertices;              // 顶点数据
    std::vector<unsigned int> m_Indices;         // 索引数据
    glm::mat4 m_LightSpaceMatrix;
};

