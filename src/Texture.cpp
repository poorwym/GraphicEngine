#include "Texture.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

Texture::Texture(const std::string& path)
		:m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr),
		m_Width(0), m_Height(0), m_BPP(0)
{
	stbi_set_flip_vertically_on_load(1);//����ͼ��ת������openGL������ͼ�Ƿ���
	m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);//4���� RGBA��4ͨ����
	//std::cout << "m_LocalBuffer " << m_LocalBuffer << std::endl;
	if (!m_LocalBuffer) {
		std::cerr << "Failed to load texture at path: " << m_FilePath << std::endl;
		return; // �����׳��쳣
	}
	GLCall(glGenTextures(1, &m_RendererID));//1����ͼ
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));//2D texture

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));//�����պ����
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));//�����պ����
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));//�պ����
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));//���

	if (m_LocalBuffer)
		stbi_image_free(m_LocalBuffer);//�ͷ��ڴ�
}

Texture::~Texture()
{
	GLCall(glDeleteTextures(1, &m_RendererID));
}

void Texture::Bind(unsigned int slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));//ʹ�ò�λslot
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}
 
void Texture::Unbind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));//���
}
