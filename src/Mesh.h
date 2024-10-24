#pragma once
#include <vector>

class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh();

    void Bind() const;                         // 绑定 VAO，准备绘制
    void Unbind() const;                       // 解除 VAO 绑定
    void Draw() const;                         // 执行绘制

    const std::vector<Vertex>& GetVertices() const;
    const std::vector<unsigned int>& GetIndices() const;

private:
    unsigned int VAO, VBO, EBO;                // VAO, VBO, EBO 管理 OpenGL 缓冲
    std::vector<Vertex> vertices;              // 顶点数据
    std::vector<unsigned int> indices;         // 索引数据

    void SetupMesh();                          // 设置 VAO/VBO/EBO
};

// 顶点结构体，包含位置信息、法线、UV 等
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};