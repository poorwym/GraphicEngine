#include "Mesh.h"
#include "Texture.h"

static void shrink(std::vector<Vertex>& vertices) {
    float max_Value = vertices[0].Position.x;
    float min_Value = vertices[0].Position.x;
    for (auto& v : vertices) {
        min_Value = std::min(min_Value, v.Position.x);
        max_Value = std::max(max_Value, v.Position.x);
        min_Value = std::min(min_Value, v.Position.y);
        max_Value = std::max(max_Value, v.Position.y);
        min_Value = std::min(min_Value, v.Position.z);
        max_Value = std::max(max_Value, v.Position.z);
    }
    float shrink_rate = std::max(abs(max_Value), abs(min_Value));

    for (auto& v : vertices) {
        v.Position.x /= shrink_rate / 3;
        v.Position.y /= shrink_rate / 3;
        v.Position.z /= shrink_rate / 3;
    }
}

Mesh::Mesh(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, Material* material)
    :m_Material(material),m_PBRMaterial(nullptr)
{
    shrink(vertices);
    m_VAO = new VertexArray();
    m_VertexBuffer = new VertexBuffer(vertices.data(), vertices.size() * sizeof(Vertex));
    VertexBufferLayout layout;
    layout.Push<float>(3); // Position: 3个浮点数
    layout.Push<float>(3); // Normal: 3个浮点数
    layout.Push<float>(2); // TexCoords: 2个浮点数

    m_IndexBuffer = new IndexBuffer(indices.data(), indices.size());

    m_VAO->AddBuffer(*m_VertexBuffer, layout);

}

Mesh::Mesh(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, PBRMaterial* PBRmaterial)
    :m_PBRMaterial(PBRmaterial),m_Material(nullptr)
{
    shrink(vertices);
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
    delete m_Material;
    delete m_PBRMaterial;
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
    if(m_PBRMaterial) m_PBRMaterial->Bind(shader);
    if(m_Material) m_Material -> Bind(shader);
    Renderer renderer;
    renderer.Draw(*m_VAO, *m_IndexBuffer, camera, shader, globalTranform);
}
