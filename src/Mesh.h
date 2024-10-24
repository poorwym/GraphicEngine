#pragma once
#include <vector>

class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh();

    void Bind() const;                         // �� VAO��׼������
    void Unbind() const;                       // ��� VAO ��
    void Draw() const;                         // ִ�л���

    const std::vector<Vertex>& GetVertices() const;
    const std::vector<unsigned int>& GetIndices() const;

private:
    unsigned int VAO, VBO, EBO;                // VAO, VBO, EBO ���� OpenGL ����
    std::vector<Vertex> vertices;              // ��������
    std::vector<unsigned int> indices;         // ��������

    void SetupMesh();                          // ���� VAO/VBO/EBO
};

// ����ṹ�壬����λ����Ϣ�����ߡ�UV ��
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};