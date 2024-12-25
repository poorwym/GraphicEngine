#include "VertexArray.h"
#include <iostream>

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
		GLCall(glEnableVertexAttribArray(i));//i表示启用顶点属性（！！非常重要一定要加，要不然黑屏）
		GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset));//见熊掌记笔记
		if(layout.GetStride() == 96 )
		std::cout << "Attirbute " << i << ": " << " Stride: " << layout.GetStride() << " Offset: " << offset << std::endl;
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}
	vb.Unbind();
	Unbind();
}

void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(m_RendererID));// 绑定顶点数组对象，即对顶点数据解释的方法
	if (glIsVertexArray(m_RendererID) == GL_FALSE) {
		std::cout << "Error: VAO is not bound or invalid!" << std::endl;
	}
}

void VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(0));
}
