#include "VertexArray.h"

#include "Renderer.h"
VertexArray::VertexArray()
{
	GLCall(glGenVertexArrays(1, &m_RendererID));//生成顶点数组对象，核心模式一定要显式定义vao(vertex array object)，兼容模式存在默认vao
}

VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &m_RendererID));//删除顶点数组对象
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	Bind();
	vb.Bind();
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); i++) {
		const auto& element = elements[i];
		GLCall(glEnableVertexAttribArray(i));//0表示启用顶点属性（！！非常重要一定要加，要不然黑屏）
		GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset));//见熊掌记笔记
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}
	
}

void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(m_RendererID));// 绑定顶点数组对象，即对顶点数据解释的方法
}

void VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(0));
}
