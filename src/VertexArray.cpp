#include "VertexArray.h"
#include <iostream>

#include "Renderer.h"
VertexArray::VertexArray()
{
	GLCall(glGenVertexArrays(1, &m_RendererID));//���ɶ���������󣬺���ģʽһ��Ҫ��ʽ����vao(vertex array object)������ģʽ����Ĭ��vao
}

VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &m_RendererID));//ɾ�������������
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	Bind();
	vb.Bind();
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); i++) {
		const auto& element = elements[i];
		GLCall(glEnableVertexAttribArray(i));//i��ʾ���ö������ԣ������ǳ���Ҫһ��Ҫ�ӣ�Ҫ��Ȼ������
		GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset));//�����ƼǱʼ�
		if(layout.GetStride() == 96 )
		std::cout << "Attirbute " << i << ": " << " Stride: " << layout.GetStride() << " Offset: " << offset << std::endl;
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}
	vb.Unbind();
	Unbind();
}

void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(m_RendererID));// �󶨶���������󣬼��Զ������ݽ��͵ķ���
	if (glIsVertexArray(m_RendererID) == GL_FALSE) {
		std::cout << "Error: VAO is not bound or invalid!" << std::endl;
	}
}

void VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(0));
}
