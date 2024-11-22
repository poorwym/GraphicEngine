#include "Mesh.h"
#include "Texture.h"
#include "depthMap.h"
static void scale(std::vector<Vertex>& vertices, float scaleRate) {
    for (Vertex& vertex : vertices) {
        vertex.Position.x *= scaleRate;
        vertex.Position.y *= scaleRate;
        vertex.Position.z *= scaleRate;
    }
}
static void BindTexture(std::vector<Vertex>& vertices, PBRMaterial* material) {
    for (Vertex& vertex : vertices) {
        vertex.TextureSlots[0] = static_cast<float>(material -> GetAlbedoMapSlot()); //0
        vertex.TextureSlots[1] = static_cast<float>(material -> GetNormalMapSlot()); //1
        vertex.TextureSlots[2] = static_cast<float>(material -> GetMetallicMapSlot()); //2
        vertex.TextureSlots[3] = static_cast<float>(material -> GetRoughnessMapSlot()); //3
        vertex.TextureSlots[4] = static_cast<float>(material -> GetAOMapSlot()); //4
        vertex.TextureSlots[5] = static_cast<float>(material->GetEmissionMapSlot()); //5
        vertex.TextureSlots[6] = static_cast<float>(material -> GetAlphaMapSlot()); //6
    }
}
static void CalcNormal(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    for (size_t i = 0; i < indices.size(); i += 3) {
        Vertex& v0 = vertices[indices[i]];
        Vertex& v1 = vertices[indices[i + 1]];
        Vertex& v2 = vertices[indices[i + 2]];

        glm::vec3 edge1 = v1.Position - v0.Position;
        glm::vec3 edge2 = v2.Position - v0.Position;

        glm::vec3 normal = glm::cross(edge1, edge2);
        normal = glm::normalize(normal);

        v0.Normal += normal;
    }
    for (Vertex& vertex : vertices) {
        vertex.Normal = glm::normalize(vertex.Normal);
    }
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
    //discard
}

Mesh::Mesh(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, PBRMaterial* PBRmaterial, float scaleRate)
    :m_PBRMaterial(PBRmaterial),m_Material(nullptr), m_LightSpaceMatrix(glm::mat4(1.0f))
{
    scale(vertices, scaleRate);
    CalcTangent(vertices, indices);//计算切线
    BindTexture(vertices, m_PBRMaterial);
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
    layout.Push<float>(7);

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

void Mesh::RenderDepthMap(Shader& shader, glm::mat4 globalTranform)
{
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
