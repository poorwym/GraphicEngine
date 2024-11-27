#include "TextureArray.h"
#include <opencv2/opencv.hpp>
#include <iostream>

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

    // 使用 OpenCV 加载图片
    cv::Mat image = cv::imread(path, cv::IMREAD_UNCHANGED); // 加载图像（包括 alpha 通道）
    if (image.empty()) {
        std::cerr << "Failed to load texture at path: " << path << std::endl;
        return;
    }
    else {
        std::cout << "Loaded texture at path: " << path << std::endl;
    }

    // 如果图像的宽高和目标纹理的宽高不同，进行调整
    if (image.cols != m_Width || image.rows != m_Height) {
        std::cerr << "TextureArray: Resizing texture from " << image.cols << "x" << image.rows
            << " to " << m_Width << "x" << m_Height << std::endl;

        // 调整大小
        cv::Mat resizedImage;
        cv::resize(image, resizedImage, cv::Size(m_Width, m_Height));

        image = resizedImage; // 更新 image 为调整大小后的图像
    }

    // 将图像数据转化为合适的格式以上传到 OpenGL
    GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID));
    GLCall(glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, m_Layers, m_Width, m_Height, 1, GL_RGBA, GL_UNSIGNED_BYTE, image.data));
    GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, 0)); // 解绑

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
