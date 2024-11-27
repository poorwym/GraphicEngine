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
