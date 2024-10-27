#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <memory>
#include "Texture.h"
#include "Material.h"
#include<Camera.h>

// 顶点结构体，包含位置信息、法线、UV 等
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

    void Bind() const;                         // 绑定 VAO，准备绘制
    void Unbind() const;                       // 解除 VAO 绑定
    void Render(Shader& shader, Camera& camera, glm::mat4 globalTranform);

    inline const std::vector<Vertex>& GetVertices() const { return vertices; };
    inline const std::vector<unsigned int>& GetIndices() const { return indices; };
    inline const IndexBuffer* GetIndexBuffer() const { return m_IndexBuffer; };


    // 删除拷贝构造函数和拷贝赋值操作符
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;


private:
    VertexArray* m_VAO;
    IndexBuffer* m_IndexBuffer;
    VertexBuffer* m_VertexBuffer;
    Material* m_Material;
    PBRMaterial*  m_PBRMaterial;

    std::vector<Vertex> vertices;              // 顶点数据
    std::vector<unsigned int> indices;         // 索引数据
};

