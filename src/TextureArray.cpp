#include "TextureArray.h"
#include "stb_image/stb_image.h"
#include <iostream>

TextureArray::TextureArray(int width, int height, int maxLayers)
    : m_RendererID(0), m_Width(width), m_Height(height), m_Layers(0), m_MaxLayers(maxLayers), m_Initialized(false)
{
    // ���� OpenGL ��������
    GLCall(glGenTextures(1, &m_RendererID));
    GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID));

    // ������������Ĵ洢�ռ䣬�����ϴ��κ���������
    GLCall(glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, m_Width, m_Height, m_MaxLayers, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));

    // �����������
    GLCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, 0)); // ���
    m_Initialized = true;
}

TextureArray::~TextureArray()
{
    if (m_Initialized)
        GLCall(glDeleteTextures(1, &m_RendererID));
}

void TextureArray::AddTexture(const std::string& path)
{
    if (m_Layers >= m_MaxLayers) {
        std::cerr << "TextureArray: Maximum layer count reached. Cannot add more textures." << std::endl;
        return;
    }

    int width, height, BPP;
    stbi_set_flip_vertically_on_load(1); // ��ת���ص���ͼ
    unsigned char* localBuffer = stbi_load(path.c_str(), &width, &height, &BPP, 4); // ����Ϊ RGBA ��ʽ

    if (!localBuffer) {
        std::cerr << "Failed to load texture at path: " << path << std::endl;
        return;
    }

    if (width != m_Width || height != m_Height) {
        std::cerr << "TextureArray: Texture dimensions do not match. Expected: "
            << m_Width << "x" << m_Height << ", Got: " << width << "x" << height << std::endl;
        stbi_image_free(localBuffer);
        return;
    }

    // �ϴ��������ݵ���������ĵ�ǰ��
    GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID));
    GLCall(glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, m_Layers, m_Width, m_Height, 1, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer));
    GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, 0)); // ���

    stbi_image_free(localBuffer); // �ͷż��ص���ͼ����
    m_Layers++; // ���ӵ�ǰ����
}

void TextureArray::Bind(unsigned int slot) const
{
    GLCall(glActiveTexture(GL_TEXTURE0 + slot)); // �����Ӧ������Ԫ
    GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID)); // ����������
}

void TextureArray::Unbind() const
{
    GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, 0)); // �����������
}