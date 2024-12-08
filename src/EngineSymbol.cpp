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
void EngineSymbol::Render(Camera& camera)
{
    m_Shader->Bind();
    m_VAO->Bind();
	GLCall(glDepthFunc(GL_ALWAYS));
	m_Shader->SetUniformMat4f("u_ProjectionView", camera.GetProjectionMatrix() * camera.GetViewMatrix());
    GLCall(glDrawArrays(GL_LINES, 0, 6));
    GLCall(glDepthFunc(GL_LESS));
    m_VAO->Unbind();
    m_Shader->Unbind();
}
