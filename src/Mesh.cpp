#include "Mesh.h"
#include "Texture.h"
#include "depthMap.h"
static void shrink(std::vector<Vertex>& vertices) {
    ;
}

static void CalcTangent(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    for (size_t i = 0; i < indices.size(); i += 3) {
        Vertex& v0 = vertices[indices[i]];
        Vertex& v1 = vertices[indices[i + 1]];
        Vertex& v2 = vertices[indices[i + 2]];

        // 计算边和UV差值
        glm::vec3 edge1 = v1.Position - v0.Position;
        glm::vec3 edge2 = v2.Position - v0.Position;
        glm::vec2 deltaUV1 = v1.TexCoords - v0.TexCoords;
        glm::vec2 deltaUV2 = v2.TexCoords - v0.TexCoords;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        glm::vec3 tangent, bitangent;

        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent = glm::normalize(tangent);

        bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent = glm::normalize(bitangent);

        // 累加切线和双切线
        v0.Tangent += tangent;
        v1.Tangent += tangent;
        v2.Tangent += tangent;

        v0.Bitangent += bitangent;
        v1.Bitangent += bitangent;
        v2.Bitangent += bitangent;
    }

    // 归一化切线和双切线
    for (auto& vertex : vertices) {
        vertex.Tangent = glm::normalize(vertex.Tangent);
        vertex.Bitangent = glm::normalize(vertex.Bitangent);
    }
}

Mesh::Mesh(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, Material* material)
    :m_Material(material),m_PBRMaterial(nullptr), m_LightSpaceMatrix(glm::mat4(1.0f))
{
    CalcTangent(vertices, indices);//计算切线
    m_Vertices = vertices;
    m_Indices = indices;
    m_VAO = new VertexArray();
    m_VertexBuffer = new VertexBuffer(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));
    VertexBufferLayout layout;
    layout.Push<float>(3); // 位置
    layout.Push<float>(3); // 法线
    layout.Push<float>(2); // 纹理坐标
    layout.Push<float>(3); // 切线
    layout.Push<float>(3); // 双切线

    m_IndexBuffer = new IndexBuffer(m_Indices.data(), m_Indices.size());

    m_VAO->AddBuffer(*m_VertexBuffer, layout);
}

Mesh::Mesh(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, PBRMaterial* PBRmaterial)
    :m_PBRMaterial(PBRmaterial),m_Material(nullptr), m_LightSpaceMatrix(glm::mat4(1.0f))
{
    CalcTangent(vertices, indices);//计算切线
    m_Vertices = vertices;
    m_Indices = indices;
    m_VAO = new VertexArray();
    m_VertexBuffer = new VertexBuffer(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));
    VertexBufferLayout layout;
    layout.Push<float>(3); // Position: 3个浮点数
    layout.Push<float>(3); // Normal: 3个浮点数
    layout.Push<float>(2); // TexCoords: 2个浮点数
    layout.Push<float>(3); // 切线
    layout.Push<float>(3); // 双切线

    m_IndexBuffer = new IndexBuffer(m_Indices.data(), m_Indices.size());

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

void Mesh::RenderDepthMap(Shader& shader, glm::mat4 globalTranform, glm::vec3 lightDir)
{

    glm::vec3 m_LightDir = glm::normalize(lightDir);


    shader.Bind();
    shader.setUniformMat4f("lightSpaceMatrix", m_LightSpaceMatrix);

    if (m_PBRMaterial) m_PBRMaterial->Bind(shader);
    if (m_Material) m_Material->Bind(shader);

    Renderer renderer;
    renderer.Draw(*m_VAO, *m_IndexBuffer, nullptr, shader, globalTranform);

    if (m_PBRMaterial) m_PBRMaterial->Unbind(shader);
    if (m_Material) m_Material->Unbind(shader);

}

void Mesh::Render(Shader& shader, Camera& camera, glm::mat4 globalTranform)
{
    shader.Bind();
    if(m_PBRMaterial) m_PBRMaterial->Bind(shader);
    if(m_Material) m_Material -> Bind(shader);

    Renderer renderer;
    renderer.Draw(*m_VAO, *m_IndexBuffer, &camera, shader, globalTranform);

    if (m_PBRMaterial) m_PBRMaterial->Unbind(shader);
    if (m_Material) m_Material->Unbind(shader);
}
