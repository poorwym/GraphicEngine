#include "Mesh.h"
#include "Texture.h"

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
{
    m_VertexBuffer = std::make_unique<VertexBuffer>(vertices.data(), vertices.size() * sizeof(Vertex));
    VertexBufferLayout layout;
    layout.Push<float>(3); // Position: 3个浮点数
    layout.Push<float>(3); // Normal: 3个浮点数
    layout.Push<float>(2); // TexCoords: 2个浮点数

    m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 6);

    m_VAO->AddBuffer(*m_VertexBuffer, layout);

    m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
    m_Shader->Bind();
    m_Shader->setUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f);
    m_Texture = std::make_unique<Texture>("res/Textures/card_box_bottom.png");
    m_Shader->setUniform1i("u_Texture", 0);//0表示texture slot（默认是0）
}
