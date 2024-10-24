#pragma once

class IndexBuffer
{
private:
	unsigned int m_RendererID;
	unsigned int m_Count;
public:
	IndexBuffer(const unsigned int* data, unsigned int count);//sizeָ�ֽ�����countָ��������
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;//const��ʾ�����޸ĳ�Ա����

	inline unsigned int GetCount() const { return m_Count; }
};