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
// 重写后的 CalcTangent 函数
static void CalcTangent(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    if (vertices.empty()) {
        std::cerr << "CalcTangent: Vertex list is empty." << std::endl;
        return;
    }

    // 初始化切线和双切线为零
    for (Vertex& vertex : vertices) {
        vertex.Tangent = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        vertex.Bitangent = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    }

    // 确保顶点数量是 3 的倍数
    if (indices.size() % 3 != 0) {
        std::cerr << "CalcTangent: Vertex count is not a multiple of 3." << std::endl;
        return;
    }

    for (size_t i = 0; i < indices.size(); i += 3) {
        Vertex& v0 = vertices.at(indices[i]);
        Vertex& v1 = vertices.at(indices[i + 1]);
        Vertex& v2 = vertices.at(indices[i + 2]);

        // 提取 Position 和 TexCoords
        glm::vec3 pos1 = glm::vec3(v0.Position);
        glm::vec3 pos2 = glm::vec3(v1.Position);
        glm::vec3 pos3 = glm::vec3(v2.Position);

        glm::vec2 uv1 = glm::vec2(v0.TexCoords);
        glm::vec2 uv2 = glm::vec2(v1.TexCoords);
        glm::vec2 uv3 = glm::vec2(v2.TexCoords);

        // 计算边缘向量
        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;

        // 计算 UV 差值
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float denominator = deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y;

        // 检查分母是否接近于零，避免除以零
        if (abs(denominator - 0.0f) <= 1e-6f) {
            // 分母接近于零，无法计算切线和双切线
            continue;
        }

        float f = 1.0f / denominator;

        // 计算切线和双切线
        glm::vec3 tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
        glm::vec3 bitangent = f * (-deltaUV2.x * edge1 + deltaUV1.x * edge2);

        // 累加切线和双切线到每个顶点
        v0.Tangent += glm::vec4(tangent, 0.0f);
        v1.Tangent += glm::vec4(tangent, 0.0f);
        v2.Tangent += glm::vec4(tangent, 0.0f);

        v0.Bitangent += glm::vec4(bitangent, 0.0f);
        v1.Bitangent += glm::vec4(bitangent, 0.0f);
        v2.Bitangent += glm::vec4(bitangent, 0.0f);
    }

    // 对切线和双切线进行标准化，并计算手性
    for (Vertex& vertex : vertices) {
        glm::vec3 tangent = glm::normalize(glm::vec3(vertex.Tangent));
        glm::vec3 bitangent = glm::normalize(glm::vec3(vertex.Bitangent));

        // 计算手性
        float handedness = (glm::dot(glm::cross(glm::vec3(vertex.Normal), tangent), bitangent) < 0.0f) ? -1.0f : 1.0f;

        // 将切线标准化并设置手性到 w 分量
        vertex.Tangent = glm::vec4(tangent, handedness);

        // 将双切线标准化（手性已存储在切线的 w 分量）
        vertex.Bitangent = glm::vec4(bitangent, 0.0f);
    }
}

static void BindTexture(std::vector<Vertex>& vertices, PBRMaterial* material) {
    for (Vertex& vertex : vertices) {
        vertex.material = material->GetMaterial();
    }
}
static void CalcNormal(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    for (size_t i = 0; i < indices.size(); i += 3) {
        Vertex& v0 = vertices[indices[i]];
        Vertex& v1 = vertices[indices[i + 1]];
        Vertex& v2 = vertices[indices[i + 2]];

        glm::vec3 edge1 = glm::vec3(v1.Position - v0.Position);
        glm::vec3 edge2 = glm::vec3(v2.Position - v0.Position);

        glm::vec3 normal = glm::cross(edge1, edge2);
        normal = glm::normalize(normal);

        v0.Normal += glm::vec4(normal, 0.0f);
    }
    for (Vertex& vertex : vertices) {
        vertex.Normal = glm::vec4(glm::normalize(glm::vec3(vertex.Normal)), 1.0f);
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
    CalcTangent(vertices, indices);
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

