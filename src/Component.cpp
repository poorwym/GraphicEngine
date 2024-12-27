#include "Component.h"
#include "ComputeShader.h"
#include "ShaderStorageBuffer.h"
#include <iostream>


Component::Component()
{
	m_LocalTransform = glm::mat4(1.0f);
}



MeshComponent::MeshComponent()
    :offset(0),m_NumVertices(0)
{
	SetType();
}

MeshComponent::~MeshComponent()
{
	for (auto& mesh : m_Meshes) {
		delete mesh;
	}
}

void MeshComponent::AddMesh(Mesh* mesh)
{
	m_Meshes.push_back(mesh);
    const std::vector<Vertex>& meshVertices = mesh->GetVertices();
    m_Vertices.insert(m_Vertices.end(), meshVertices.begin(), meshVertices.end());
    const std::vector<unsigned int>& meshIndices = mesh->GetIndices();
    for (unsigned int index : meshIndices) {
        m_Indices.push_back(index + offset);
    }
    offset += meshVertices.size();
}

void MeshComponent::RenderDepthMap(Shader& shader, glm::mat4 globalTransform)
{
	glm::mat4 m_GlobalTransform = globalTransform * m_LocalTransform;
}

void MeshComponent::Render(Shader& shader, Camera& camera, glm::mat4 globalTransform)
{
	glm::mat4 m_GlobalTransform = globalTransform * m_LocalTransform;
}

void MeshComponent::Update(float deltaTime)
{
}


std::vector<Vertex>* MeshComponent::GetVertices(const glm::mat4& globalTransform)
{
    //glm::mat4 m_GlobalTransform = globalTransform * m_LocalTransform;
    static ComputeShader* computeShader = new ComputeShader("res/shaders/ComputeShaders/ModelTransform.glsl");
    m_NumVertices = m_Vertices.size();
    m_TransformedVertices.resize(m_NumVertices);
    // 创建顶点SSBO并上传数据
    m_InputSSBO = ShaderStorageBuffer(m_Vertices.data(), sizeof(Vertex) * m_NumVertices, 1);
    m_OutputSSBO = ShaderStorageBuffer(m_TransformedVertices.data(), sizeof(Vertex) * m_NumVertices, 2);
    // 绑定顶点SSBO
    m_InputSSBO.Bind();
    m_OutputSSBO.Bind();
    // 计算工作组数量
    unsigned int localSizeX = 512;
    unsigned int numGroups = static_cast<unsigned int>(ceil(m_NumVertices / (float)localSizeX));

    // 绑定计算着色器并设置Uniform
    computeShader->Bind();
    computeShader->SetUniformMat4f("u_Model", globalTransform);
    computeShader->SetUniformMat3f("normalMatrix", glm::transpose(glm::inverse(glm::mat3(globalTransform))));
    computeShader->SetUniform1i("numVertices", m_NumVertices);

    // 调度计算着色器
    computeShader->Dispatch(numGroups, 1, 1);

    // 内存屏障，确保计算着色器的写操作完成
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // 解绑计算着色器
    computeShader->Unbind();
    // 解绑顶点SSBO
    m_InputSSBO.Unbind();
    m_OutputSSBO.Unbind();

    // 读取更新后的顶点数据从GPU
    void* mappedData = m_OutputSSBO.Map(GL_READ_ONLY);
    if (mappedData)
    {
        std::memcpy(m_TransformedVertices.data(), mappedData, sizeof(Vertex) * m_NumVertices);
        m_OutputSSBO.Unmap();
    }
    else
    {
        std::cerr << "Failed to map SSBO for reading." << std::endl;
    }
    return &m_TransformedVertices; // 返回更新后的顶点数据
}

std::vector<unsigned int>* MeshComponent::GetIndices()
{
	return &m_Indices;
}
