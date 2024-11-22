#pragma once
#include <vector>
#include <string>
#include "Renderer.h"

class TextureArray
{
private:
    unsigned int m_RendererID;     // OpenGL �������� ID
    int m_Width, m_Height;         // ÿ������Ŀ�͸�
    int m_Layers;                  // ��ǰ�Ĳ���
    int m_MaxLayers;               // ���֧�ֵĲ���
    bool m_Initialized;            // �Ƿ��ѳ�ʼ��

public:
    TextureArray(int width, int height, int maxLayers);
    ~TextureArray();

    void AddTexture(const std::string& path); // ���һ������������
    void Bind(unsigned int slot = 0) const;   // ����������
    void Unbind() const;                      // �����������

    inline int GetWidth() const { return m_Width; }
    inline int GetHeight() const { return m_Height; }
    inline int GetLayers() const { return m_Layers; }
    inline unsigned int GetTextureID() const { return m_RendererID; }
};