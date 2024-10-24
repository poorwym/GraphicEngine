#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <memory>
#include "Renderer.h"

// ����ṹ�壬����λ����Ϣ�����ߡ�UV ��
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh();

    void Bind() const;                         // �� VAO��׼������
    void Unbind() const;                       // ��� VAO ��

    inline const std::vector<Vertex>& GetVertices() const { return vertices; };
    inline const std::vector<unsigned int>& GetIndices() const { return indices; };

private:
    std::unique_ptr<VertexArray> m_VAO;
    std::unique_ptr<IndexBuffer> m_IndexBuffer;
    std::unique_ptr<VertexBuffer> m_VertexBuffer;
    std::unique_ptr<Shader> m_Shader;
    std::unique_ptr<Texture> m_Texture;

    std::vector<Vertex> vertices;              // ��������
    std::vector<unsigned int> indices;         // ��������

};

