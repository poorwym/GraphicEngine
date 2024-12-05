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



CPUTexture::CPUTexture() {
    // 构造函数
}

CPUTexture::CPUTexture(const std::string& filename) {
    LoadTexture(filename);
}

bool CPUTexture::LoadTexture(const std::string& filename) {
    // 使用 OpenCV 加载图像
    textureImage = cv::imread(filename, cv::IMREAD_UNCHANGED);

    if (textureImage.empty()) {
        std::cerr << "Error: Failed to load texture image: " << filename << std::endl;
        return false;
    }
    else {
        std::cout << "Loaded texture image: " << filename << std::endl;
    }

    // 将图像转换为浮点型，范围 [0,1]
    textureImage.convertTo(textureImage, CV_32FC(textureImage.channels()), 1.0 / 255.0);

    return true;
}

int CPUTexture::GetWidth() const {
    return textureImage.cols;
}

int CPUTexture::GetHeight() const {
    return textureImage.rows;
}

int CPUTexture::GetChannelCount() const {
    return textureImage.channels();
}

cv::Vec3f CPUTexture::Sample(const cv::Vec2f& texCoord) const {
    if (textureImage.empty()) {
        return cv::Vec3f(0.0f, 0.0f, 0.0f);
    }

    // 确保纹理坐标在 [0,1] 范围内
    float u = texCoord[0] - floor(texCoord[0]);
    float v = texCoord[1] - floor(texCoord[1]);

    // 将纹理坐标转换为图像像素坐标
    float x = u * (textureImage.cols - 1);
    float y = (1.0f - v) * (textureImage.rows - 1); // OpenCV 的坐标系 y 轴向下

    // 双线性插值采样
    int x0 = static_cast<int>(floor(x));
    int x1 = std::min(x0 + 1, textureImage.cols - 1);
    int y0 = static_cast<int>(floor(y));
    int y1 = std::min(y0 + 1, textureImage.rows - 1);

    float dx = x - x0;
    float dy = y - y0;

    cv::Vec3f c00 = textureImage.at<cv::Vec3f>(y0, x0);
    cv::Vec3f c10 = textureImage.at<cv::Vec3f>(y0, x1);
    cv::Vec3f c01 = textureImage.at<cv::Vec3f>(y1, x0);
    cv::Vec3f c11 = textureImage.at<cv::Vec3f>(y1, x1);

    cv::Vec3f c0 = c00 * (1.0f - dx) + c10 * dx;
    cv::Vec3f c1 = c01 * (1.0f - dx) + c11 * dx;
    cv::Vec3f c = c0 * (1.0f - dy) + c1 * dy;

    return c;
}

float CPUTexture::GetGrayValue(const cv::Vec2f& texCoord) const {
    if (textureImage.empty()) {
        return 0.0f;
    }

    // 确保纹理坐标在 [0,1] 范围内
    float u = texCoord[0] - floor(texCoord[0]);
    float v = texCoord[1] - floor(texCoord[1]);

    // 将纹理坐标转换为图像像素坐标
    float x = u * (textureImage.cols - 1);
    float y = (1.0f - v) * (textureImage.rows - 1); // OpenCV 的坐标系 y 轴向下

    // 双线性插值采样灰度值
    int x0 = static_cast<int>(floor(x));
    int x1 = std::min(x0 + 1, textureImage.cols - 1);
    int y0 = static_cast<int>(floor(y));
    int y1 = std::min(y0 + 1, textureImage.rows - 1);

    float dx = x - x0;
    float dy = y - y0;

    float c00 = cv::norm(textureImage.at<cv::Vec3f>(y0, x0)) / sqrt(3.0f);
    float c10 = cv::norm(textureImage.at<cv::Vec3f>(y0, x1)) / sqrt(3.0f);
    float c01 = cv::norm(textureImage.at<cv::Vec3f>(y1, x0)) / sqrt(3.0f);
    float c11 = cv::norm(textureImage.at<cv::Vec3f>(y1, x1)) / sqrt(3.0f);

    float c0 = c00 * (1.0f - dx) + c10 * dx;
    float c1 = c01 * (1.0f - dx) + c11 * dx;
    float c = c0 * (1.0f - dy) + c1 * dy;

    return c;
}