#pragma once
#include <vector>
#include <string>
#include "Renderer.h"

class TextureArray
{
private:
    unsigned int m_RendererID;     // OpenGL 纹理数组 ID
    int m_Width, m_Height;         // 每层纹理的宽和高
    int m_Layers;                  // 当前的层数
    int m_MaxLayers;               // 最大支持的层数
    bool m_Initialized;            // 是否已初始化

public:
    TextureArray(int width, int height, int maxLayers);
    ~TextureArray();

    void AddTexture(const std::string& path); // 添加一张纹理到数组中
    void Bind(unsigned int slot = 0) const;   // 绑定纹理数组
    void Unbind() const;                      // 解绑纹理数组

    inline int GetWidth() const { return m_Width; }
    inline int GetHeight() const { return m_Height; }
    inline int GetLayers() const { return m_Layers; }
    inline unsigned int GetTextureID() const { return m_RendererID; }
};