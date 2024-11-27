#include "Texture.h"
#include "opencv2/opencv.hpp"
#include <iostream>

Texture::Texture(const std::string& path)
    : m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr),
    m_Width(0), m_Height(0), m_BPP(0)
{
    // 使用 OpenCV 加载图像
    cv::Mat image = cv::imread(path, cv::IMREAD_UNCHANGED); // 加载图像（包括 alpha 通道）
    if (image.empty()) {
        std::cerr << "Failed to load texture at path: " << m_FilePath << std::endl;
        return; // 或者抛出异常
    }
    else {
        std::cout << "Loaded texture at path: " << m_FilePath << std::endl;
    }

    // OpenCV 加载的图像数据转换为 OpenGL 可用的格式
    m_Width = image.cols;
    m_Height = image.rows;
    m_BPP = image.channels(); // 通道数，RGBA = 4

    // OpenCV 读取的图像数据已经是正确格式，直接使用它
    m_LocalBuffer = image.data;

    // 创建 OpenGL 纹理
    GLCall(glGenTextures(1, &m_RendererID));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

    // 设置纹理参数
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    // 上传纹理数据到 GPU
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
    GLCall(glBindTexture(GL_TEXTURE_2D, 0)); // 解绑

    // 释放 OpenCV 图像数据（如果是深拷贝，OpenCV 会管理图像内存）
    // 如果 OpenCV 图像是浅拷贝，它会在 `Texture` 析构时被释放，所以无需调用 `cv::Mat` 的 `release`。
}

Texture::~Texture()
{
    GLCall(glDeleteTextures(1, &m_RendererID));
}

void Texture::Bind(unsigned int slot) const
{
    GLCall(glActiveTexture(GL_TEXTURE0 + slot)); // 激活纹理单元
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID)); // 绑定纹理
}

void Texture::Unbind() const
{
    GLCall(glBindTexture(GL_TEXTURE_2D, 0)); // 解绑纹理
}
