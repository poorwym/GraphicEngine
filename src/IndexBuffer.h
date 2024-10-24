#pragma once

class IndexBuffer
{
private:
	unsigned int m_RendererID;
	unsigned int m_Count;
public:
	IndexBuffer(const unsigned int* data, unsigned int count);//size指字节数，count指索引数量
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;//const表示不会修改成员变量

	inline unsigned int GetCount() const { return m_Count; }
};