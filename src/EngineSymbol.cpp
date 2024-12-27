#include "EngineSymbol.h"
#include "Renderer.h"
std::vector<float> axisVertices = {
	0.0, 0.0, 0.0,	 1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,	 1.0, 0.0, 0.0,

	0.0, 0.0, 0.0,	 0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,	 0.0, 1.0, 0.0,

	0.0, 0.0, 0.0,	 0.0, 0.0, 1.0,
	0.0, 0.0, 1.0,	 0.0, 0.0, 1.0
};

EngineSymbol::EngineSymbol()
	:m_VBO(nullptr), m_VAO(nullptr),m_Shader(nullptr)
{
	m_VAO = new VertexArray();
	m_VBO = new VertexBuffer(axisVertices.data(), axisVertices.size() * sizeof(float));

	VertexBufferLayout layout;
	layout.Push<float>(3);
	layout.Push<float>(3);

	m_VAO->AddBuffer(*m_VBO, layout);

	m_Shader = new Shader("res/shaders/EngineShader/Axis.glsl");
}

void EngineSymbol::Render(Camera& camera, float screenWidth, float screenHeight)
{
    m_Shader->Bind();
    m_VAO->Bind();
    glViewport(0, 0, screenWidth, screenHeight);
    // ������Ȳ��ԣ�ȷ��������������ǰ��
    GLCall(glDepthFunc(GL_ALWAYS));

    glm::mat4 projection = camera.GetProjectionMatrix();
    glm::mat4 view = camera.GetViewMatrix();

    // �����������С
    float axisLength = 3.0f; // 100���س���
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(axisLength, axisLength, axisLength));
    // ��ϱ任����
    glm::mat4 projectionView = projection * view * scale;

    m_Shader->SetUniformMat4f("u_ProjectionView", projectionView );

    // ����������
    glLineWidth(5.0f);
    GLCall(glDrawArrays(GL_LINES, 0, 6));

    // �ָ�Ĭ����Ⱥ���
    GLCall(glDepthFunc(GL_LESS));

    m_VAO->Unbind();
    m_Shader->Unbind();
}