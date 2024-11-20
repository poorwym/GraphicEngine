#include "FrameBuffer.h"
#include <iostream>

FrameBuffer::FrameBuffer(unsigned int width, unsigned int height)
    : m_RendererID(0), m_Width(width), m_Height(height)
{
    // ����֡����
    GLCall(glGenFramebuffers(1, &m_RendererID));
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));

}

FrameBuffer::~FrameBuffer()
{
    GLCall(glDeleteFramebuffers(1, &m_RendererID));
}

void FrameBuffer::Bind() const
{
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));
}

void FrameBuffer::Unbind() const
{
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}


void FrameBuffer::CheckFramebufferComplete() const
{
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Error: Framebuffer is not complete!" << std::endl;
    }
}

DepthMapFBO::DepthMapFBO(unsigned int width, unsigned int height)
    :FrameBuffer(width, height)
{

    // �������������
    unsigned int depthTexture;
    GLCall(glGenTextures(1, &depthTexture));
    GLCall(glBindTexture(GL_TEXTURE_2D, depthTexture));
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLCall(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor));

    // �����������֡����
    GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0));
    m_TextureID = depthTexture;

    // ����Ҫ��ɫ����
    GLCall(glDrawBuffer(GL_NONE));
    GLCall(glReadBuffer(GL_NONE));

    // ���֡����������
    CheckFramebufferComplete();

    // ���֡����
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

DepthMapFBO::~DepthMapFBO()
{
    GLCall(glDeleteFramebuffers(1, &m_RendererID));
    GLCall(glDeleteTextures(1, &m_TextureID));
}

void DepthMapFBO::BindTexture(unsigned int slot) const
{
    GLCall(glActiveTexture(GL_TEXTURE0 + slot));//ʹ�ò�λslot
    GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureID));
}

// CubeMapFBO ���캯��
CubeMapFBO::CubeMapFBO(unsigned int width, unsigned int height)
    : FrameBuffer(width, height), m_TextureID(0)
{
    // ������������ͼ
    GLCall(glGenTextures(1, &m_TextureID));
    GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID));

    for (unsigned int i = 0; i < 6; ++i)
    {
        // Ϊÿ�������洢�ռ�
        GLCall(glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT24,
            m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL
        ));
    }

    // ������������ͼ����
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

    // ��֡�������
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));

    // ���ӵ�һ����������ͼ����Ϊ��ȸ���
    GLCall(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_TextureID, 0));

    // ����Ҫ��ɫ����
    GLCall(glDrawBuffer(GL_NONE));
    GLCall(glReadBuffer(GL_NONE));

    // ���֡����������
    CheckFramebufferComplete();

    // ���֡����
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

// CubeMapFBO ��������
CubeMapFBO::~CubeMapFBO()
{
    GLCall(glDeleteFramebuffers(1, &m_RendererID));
    GLCall(glDeleteTextures(1, &m_TextureID));
}

// ��ָ������������ͼ��������Ⱦ
void CubeMapFBO::BindFace(unsigned int face) const
{
    if (face >= 6)
    {
        std::cerr << "Error: CubeMapFBO::BindFace - Face index out of range (0-5)" << std::endl;
        return;
    }

    // ��֡�������
    Bind();

    // ����ָ������������ͼ����Ϊ��ȸ���
    GLCall(glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
        m_TextureID, 0
    ));

    // ���֡����������
    CheckFramebufferComplete();
}

// ����������ͼ��ָ�������
void CubeMapFBO::BindTexture(unsigned int slot) const
{
    GLCall(glActiveTexture(GL_TEXTURE0 + slot)); // ʹ��ָ���������
    GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID));
}

ColorFBO::ColorFBO(unsigned int width, unsigned int height)
    : FrameBuffer(width, height), m_ColorTextureID(0), m_DepthTextureID(0)
{
    // ������ɫ����
    GLCall(glGenTextures(1, &m_ColorTextureID));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_ColorTextureID));
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    // �����������
    GLCall(glGenTextures(1, &m_DepthTextureID));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_DepthTextureID));
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    // ����������ӵ�֡�������ȸ�����
    GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTextureID, 0));

    // ����ɫ�����ӵ�֡�������ɫ������
    GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorTextureID, 0));
    // ָ�����ƻ�����
    GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    GLCall(glDrawBuffers(1, DrawBuffers));

    // ���֡����������
    CheckFramebufferComplete();

    // ���֡����
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

// ColorFBO ��������
ColorFBO::~ColorFBO()
{
    GLCall(glDeleteFramebuffers(1, &m_RendererID));
    GLCall(glDeleteTextures(1, &m_ColorTextureID));
    GLCall(glDeleteTextures(1, &m_DepthTextureID));

}

// ����ɫ����ָ�������
void ColorFBO::BindTexture(unsigned int slot) const
{
    GLCall(glActiveTexture(GL_TEXTURE0 + slot)); // ʹ��ָ���������
    GLCall(glBindTexture(GL_TEXTURE_2D, m_ColorTextureID));
}

void ColorFBO::BindDepthTexture(unsigned int slot) const
{
    GLCall(glActiveTexture(GL_TEXTURE0 + slot)); // ʹ��ָ���������
    GLCall(glBindTexture(GL_TEXTURE_2D, m_DepthTextureID));
}
