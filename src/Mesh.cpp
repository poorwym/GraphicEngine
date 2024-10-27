#include "Mesh.h"
#include "Texture.h"
#include "depthMap.h"

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

static glm::mat4 GetLightSpaceMatrix(glm::vec3 lightDir, glm::mat4 globalTransform) {
    // 定义场景中心和大小
    glm::vec3 sceneCenter = globalTransform * glm::vec4(1.0f);
    float sceneRadius = 100.f;

    // 设置光源的位置
    float distanceFromScene = sceneRadius * 2.0f; // 根据需要调整
    glm::vec3 lightPos = sceneCenter - lightDir * distanceFromScene;

    // 确定上向量
    glm::vec3 up;
    if (abs(lightDir.x) < 0.001f && abs(lightDir.z) < 0.001f) {
        up = glm::vec3(0.0f, 0.0f, 1.0f);
    }
    else {
        up = glm::vec3(0.0f, 1.0f, 0.0f);
    }
    // 计算光的视图矩阵
    glm::mat4 lightView = glm::lookAt(lightPos, sceneCenter, up);
    // 定义正交投影的参数
    float orthoSize = sceneRadius * 2.0f;
    float nearPlane = 0.1f;
    float farPlane = distanceFromScene + sceneRadius * 2.0f;

    glm::mat4 lightProjection = glm::ortho(
        -orthoSize, orthoSize,
        -orthoSize, orthoSize,
        nearPlane, farPlane
    );

    // 计算光空间矩阵
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;
    return lightSpaceMatrix;
}

Mesh::Mesh(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, Material* material)
    :m_Material(material),m_PBRMaterial(nullptr), m_LightSpaceMatrix(glm::mat4(1.0f))
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
    m_FrameBuffer = new FrameBuffer(1920, 1280);
}

Mesh::Mesh(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, PBRMaterial* PBRmaterial)
    :m_PBRMaterial(PBRmaterial),m_Material(nullptr), m_LightSpaceMatrix(glm::mat4(1.0f))
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
    m_FrameBuffer = new FrameBuffer(1920, 1280);
}

Mesh::~Mesh()
{
    delete m_VAO;
    delete m_VertexBuffer;
    delete m_IndexBuffer;
    delete m_Material;
    delete m_PBRMaterial;
    delete m_FrameBuffer;
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
    m_FrameBuffer->Bind();

    glm::vec3 m_LightDir = glm::normalize(lightDir);

    m_LightSpaceMatrix = GetLightSpaceMatrix(m_LightDir, globalTranform);

    shader.Bind();
    shader.setUniformMat4f("lightSpaceMatrix", m_LightSpaceMatrix);

    if (m_PBRMaterial) m_PBRMaterial->Bind(shader);
    if (m_Material) m_Material->Bind(shader);

    Renderer renderer;
    renderer.Draw(*m_VAO, *m_IndexBuffer, nullptr, shader, globalTranform, &m_LightSpaceMatrix);

    if (m_PBRMaterial) m_PBRMaterial->Unbind(shader);
    if (m_Material) m_Material->Unbind(shader);

    m_FrameBuffer->Unbind();
}

void Mesh::Render(Shader& shader, Camera& camera, glm::mat4 globalTranform)
{
    shader.Bind();
    if(m_PBRMaterial) m_PBRMaterial->Bind(shader);
    if(m_Material) m_Material -> Bind(shader);

    m_FrameBuffer->BindTexture(7, FrameBuffer::DEPTH_ATTACHMENT);
    shader.setUniform1i("shadowMap", 7);

    Renderer renderer;
    renderer.Draw(*m_VAO, *m_IndexBuffer, &camera, shader, globalTranform);

    if (m_PBRMaterial) m_PBRMaterial->Unbind(shader);
    if (m_Material) m_Material->Unbind(shader);
}
