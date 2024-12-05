#include "Texture.h"
#include "opencv2/opencv.hpp"
#include <iostream>

Texture::Texture(const std::string& path)
    : m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr),
    m_Width(0), m_Height(0), m_BPP(0)
{
    // ʹ�� OpenCV ����ͼ��
    cv::Mat image = cv::imread(path, cv::IMREAD_UNCHANGED); // ����ͼ�񣨰��� alpha ͨ����
    if (image.empty()) {
        std::cerr << "Failed to load texture at path: " << m_FilePath << std::endl;
        return; // �����׳��쳣
    }
    else {
        std::cout << "Loaded texture at path: " << m_FilePath << std::endl;
    }

    // OpenCV ���ص�ͼ������ת��Ϊ OpenGL ���õĸ�ʽ
    m_Width = image.cols;
    m_Height = image.rows;
    m_BPP = image.channels(); // ͨ������RGBA = 4

    // OpenCV ��ȡ��ͼ�������Ѿ�����ȷ��ʽ��ֱ��ʹ����
    m_LocalBuffer = image.data;

    // ���� OpenGL ����
    GLCall(glGenTextures(1, &m_RendererID));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

    // �����������
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    // �ϴ��������ݵ� GPU
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
    GLCall(glBindTexture(GL_TEXTURE_2D, 0)); // ���

    // �ͷ� OpenCV ͼ�����ݣ�����������OpenCV �����ͼ���ڴ棩
    // ��� OpenCV ͼ����ǳ������������ `Texture` ����ʱ���ͷţ������������ `cv::Mat` �� `release`��
}

Texture::~Texture()
{
    GLCall(glDeleteTextures(1, &m_RendererID));
}

void Texture::Bind(unsigned int slot) const
{
    GLCall(glActiveTexture(GL_TEXTURE0 + slot)); // ��������Ԫ
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID)); // ������
}

void Texture::Unbind() const
{
    GLCall(glBindTexture(GL_TEXTURE_2D, 0)); // �������
}



CPUTexture::CPUTexture() {
    // ���캯��
}

CPUTexture::CPUTexture(const std::string& filename) {
    LoadTexture(filename);
}

bool CPUTexture::LoadTexture(const std::string& filename) {
    // ʹ�� OpenCV ����ͼ��
    textureImage = cv::imread(filename, cv::IMREAD_UNCHANGED);

    if (textureImage.empty()) {
        std::cerr << "Error: Failed to load texture image: " << filename << std::endl;
        return false;
    }
    else {
        std::cout << "Loaded texture image: " << filename << std::endl;
    }

    // ��ͼ��ת��Ϊ�����ͣ���Χ [0,1]
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

    // ȷ������������ [0,1] ��Χ��
    float u = texCoord[0] - floor(texCoord[0]);
    float v = texCoord[1] - floor(texCoord[1]);

    // ����������ת��Ϊͼ����������
    float x = u * (textureImage.cols - 1);
    float y = (1.0f - v) * (textureImage.rows - 1); // OpenCV ������ϵ y ������

    // ˫���Բ�ֵ����
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

    // ȷ������������ [0,1] ��Χ��
    float u = texCoord[0] - floor(texCoord[0]);
    float v = texCoord[1] - floor(texCoord[1]);

    // ����������ת��Ϊͼ����������
    float x = u * (textureImage.cols - 1);
    float y = (1.0f - v) * (textureImage.rows - 1); // OpenCV ������ϵ y ������

    // ˫���Բ�ֵ�����Ҷ�ֵ
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