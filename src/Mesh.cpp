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
static void CalcTangent(std::vector<Vertex>& vertices) {
    // 首先初始化切线和双切线为零
    for (Vertex& vertex : vertices) {
        vertex.Tangent = glm::vec3(0.0f);
        vertex.Bitangent = glm::vec3(0.0f);
    }

    // 假设顶点按三角形列表排列，每三个顶点组成一个三角形
    for (size_t i = 0; i < vertices.size(); i += 3) {
        Vertex& v0 = vertices[i + 0];
        Vertex& v1 = vertices[i + 1];
        Vertex& v2 = vertices[i + 2];

        // 计算边缘向量
        glm::vec3 edge1 = v1.Position - v0.Position;
        glm::vec3 edge2 = v2.Position - v0.Position;

        // 计算 UV 差值
        glm::vec2 deltaUV1 = v1.TexCoords - v0.TexCoords;
        glm::vec2 deltaUV2 = v2.TexCoords - v0.TexCoords;

        float denominator = deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y;
        float f = 1.0f / denominator;

        // 检查分母是否为零，避免除以零
        if (std::isfinite(f)) {
            glm::vec3 tangent;
            tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
            tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
            tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

            glm::vec3 bitangent;
            bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
            bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
            bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

            // 累加切线和双切线
            v0.Tangent += tangent;
            v1.Tangent += tangent;
            v2.Tangent += tangent;

            v0.Bitangent += bitangent;
            v1.Bitangent += bitangent;
            v2.Bitangent += bitangent;
        }
    }
    // 对切线和双切线进行标准化
    for (Vertex& vertex : vertices) {
        vertex.Tangent = glm::normalize(vertex.Tangent);
        vertex.Bitangent = glm::normalize(vertex.Bitangent);
    }
}
static void BindTexture(std::vector<Vertex>& vertices, PBRMaterial* material) {
    for (Vertex& vertex : vertices) {
        vertex.material.AlbedoMapIndex = static_cast<float>(material -> GetAlbedoMapSlot()); //0
        vertex.material.NormalMapIndex = static_cast<float>(material -> GetNormalMapSlot()); //1
        vertex.material.MetallicMapIndex = static_cast<float>(material -> GetMetallicMapSlot()); //2
        vertex.material.RoughnessMapIndex = static_cast<float>(material -> GetRoughnessMapSlot()); //3
        vertex.material.AOMapIndex = static_cast<float>(material -> GetAOMapSlot()); //4
        vertex.material.EmissionMapIndex = static_cast<float>(material->GetEmissionMapSlot()); //5
        vertex.material.AlphaMapIndex = static_cast<float>(material -> GetAlphaMapSlot()); //6
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



Mesh::Mesh(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, Material* material)
    :m_Material(material),m_PBRMaterial(nullptr), m_LightSpaceMatrix(glm::mat4(1.0f))
{
    //discard
}

Mesh::Mesh(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, PBRMaterial* PBRmaterial, float scaleRate)
    :m_PBRMaterial(PBRmaterial),m_Material(nullptr), m_LightSpaceMatrix(glm::mat4(1.0f))
{
    scale(vertices, scaleRate);
    BindTexture(vertices, m_PBRMaterial);
    CalcTangent(vertices);
    m_Vertices = vertices;
    m_Indices = indices;
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

