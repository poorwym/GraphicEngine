#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <memory>
#include "Texture.h"
#include "Material.h"
#include "FrameBuffer.h"
#include<Camera.h>

// ����ṹ�壬����λ����Ϣ�����ߡ�UV ��
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class Mesh {
public:
    Mesh(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, Material* material);
    Mesh(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, PBRMaterial* PBRmaterial);
    ~Mesh();

    void Bind() const;                         // �� VAO��׼������
    void Unbind() const;                       // ��� VAO ��
    void RenderDepthMap(Shader& shader, glm::mat4 globalTranform, glm::vec3 lightDir);
    void Render(Shader& shader, Camera& camera, glm::mat4 globalTranform);

    inline const std::vector<Vertex>& GetVertices() const { return vertices; };
    inline const std::vector<unsigned int>& GetIndices() const { return indices; };
    inline const IndexBuffer* GetIndexBuffer() const { return m_IndexBuffer; };


private:
    VertexArray* m_VAO;
    IndexBuffer* m_IndexBuffer;
    VertexBuffer* m_VertexBuffer;
    FrameBuffer* m_FrameBuffer;
    Material* m_Material;
    PBRMaterial*  m_PBRMaterial;

    std::vector<Vertex> vertices;              // ��������
    std::vector<unsigned int> indices;         // ��������
    glm::mat4 m_LightSpaceMatrix;
};

