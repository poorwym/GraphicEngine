#include "Mesh.h"
#include "Texture.h"

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, Material* material)
    :m_Material(material)
{
    m_VAO = new VertexArray();
    m_VertexBuffer = new VertexBuffer(vertices.data(), vertices.size() * sizeof(Vertex));
    VertexBufferLayout layout;
    layout.Push<float>(3); // Position: 3个浮点数
    layout.Push<float>(3); // Normal: 3个浮点数
    layout.Push<float>(2); // TexCoords: 2个浮点数

    m_IndexBuffer = new IndexBuffer(indices.data(), indices.size());

    m_VAO->AddBuffer(*m_VertexBuffer, layout);

}

Mesh::~Mesh()
{
    delete m_VAO;
    delete m_VertexBuffer;
    delete m_IndexBuffer;
    delete m_Material; // 如果 m_Material 的生命周期应由 Mesh 管理
}

void Mesh::Bind() const
{
    m_VAO->Bind();
    m_IndexBuffer->Bind();
}

void Mesh::Unbind() const
{
    m_VAO->Unbind();
    m_IndexBuffer->Unbind();
}

void Mesh::Render(Shader& shader, Camera& camera, glm::mat4 globalTranform)
{
    shader.Bind();
    m_Material -> Bind(shader);
    Renderer renderer;
    renderer.Draw(*m_VAO, *m_IndexBuffer, camera, shader, globalTranform);
}

void Mesh::setMaterial(Texture* diffuse, Texture* normal, Texture* specular)
{
    m_Material = new Material(diffuse, normal, specular);
}
