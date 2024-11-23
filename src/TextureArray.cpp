#include "TextureArray.h"
#include "stb_image/stb_image.h"
#include <iostream>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image/stb_image_resize2.h"

TextureArray::TextureArray(int width, int height, int maxLayers)
    : m_RendererID(0), m_Width(width), m_Height(height), m_Layers(0), m_MaxLayers(maxLayers), m_Initialized(false)
{
    // 创建 OpenGL 纹理数组
    GLCall(glGenTextures(1, &m_RendererID));
    GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID));

    // 分配纹理数组的存储空间，但不上传任何纹理数据
    GLCall(glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, m_Width, m_Height, m_MaxLayers, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));

    // 设置纹理参数
    GLCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, 0)); // 解绑
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
    stbi_set_flip_vertically_on_load(1); // 翻转加载的贴图
    unsigned char* localBuffer = stbi_load(path.c_str(), &width, &height, &BPP, 4); // 加载为 RGBA 格式

    if (!localBuffer) {
        std::cerr << "Failed to load texture at path: " << path << std::endl;
        return;
    }
    else {
        std::cout << "Loaded texture at path: " << path << std::endl;
    }

    if (width != m_Width || height != m_Height) {
        std::cerr << "TextureArray: Resizing texture from " << width << "x" << height
            << " to " << m_Width << "x" << m_Height << std::endl;

        // 创建一个新的缓冲区，用于存储调整大小后的纹理数据
        unsigned char* resizedBuffer = new unsigned char[m_Width * m_Height * 4]; // RGBA 格式

        // 调整大小
        unsigned char* result = stbir_resize_uint8_linear(
            localBuffer, width, height, 0,                  // 输入图像数据及其宽度、高度
            resizedBuffer, m_Width, m_Height, 0,           // 输出图像数据及其宽度、高度
            STBIR_RGBA                                              // 每像素的通道数（RGBA）
        );

        // 释放原始加载的纹理数据
        stbi_image_free(localBuffer);

        if (result == 0) {
            std::cerr << "TextureArray: Failed to resize texture." << std::endl;
            delete[] resizedBuffer; // 释放分配的缓冲区
            return;
        }

        localBuffer = resizedBuffer; // 将调整后的数据指向 localBuffer
    }

    // 上传纹理数据到纹理数组的当前层
    GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID));
    GLCall(glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, m_Layers, m_Width, m_Height, 1, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer));
    GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, 0)); // 解绑

    // 释放加载的或调整大小后的贴图数据
    stbi_image_free(localBuffer);

    m_Layers++; // 增加当前层数
}

void TextureArray::Bind(unsigned int slot) const
{
    GLCall(glActiveTexture(GL_TEXTURE0 + slot)); // 激活对应的纹理单元
    GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID)); // 绑定纹理数组
}

void TextureArray::Unbind() const
{
    GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, 0)); // 解绑纹理数组
}