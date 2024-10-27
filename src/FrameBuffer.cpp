#include "FrameBuffer.h"
#include <iostream>

FrameBuffer::FrameBuffer(unsigned int width, unsigned int height)
    : m_RendererID(0), m_Width(width), m_Height(height), m_RBO(0)
{
    // 生成帧缓冲
    GLCall(glGenFramebuffers(1, &m_RendererID));
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));

    // 创建深度纹理附件
    unsigned int depthTexture;
    GLCall(glGenTextures(1, &depthTexture));
    GLCall(glBindTexture(GL_TEXTURE_2D, depthTexture));
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLCall(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor));

    // 附加深度纹理到帧缓冲
    GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0));
    m_Textures[DEPTH_ATTACHMENT] = depthTexture;

    // 不需要颜色缓冲
    GLCall(glDrawBuffer(GL_NONE));
    GLCall(glReadBuffer(GL_NONE));

    // 检查帧缓冲完整性
    CheckFramebufferComplete();

    // 解绑帧缓冲
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

FrameBuffer::~FrameBuffer()
{
    GLCall(glDeleteFramebuffers(1, &m_RendererID));

    for (auto& pair : m_Textures)
    {
        GLCall(glDeleteTextures(1, &pair.second));
    }

    if (m_RBO != 0)
    {
        GLCall(glDeleteRenderbuffers(1, &m_RBO));
    }
}

void FrameBuffer::Bind() const
{
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));
}

void FrameBuffer::Unbind() const
{
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void FrameBuffer::BindTexture(unsigned int slot, AttachmentType type) const
{
    auto it = m_Textures.find(type);
    if (it != m_Textures.end())
    {
        GLCall(glActiveTexture(GL_TEXTURE0 + slot));
        GLCall(glBindTexture(GL_TEXTURE_2D, it->second));
    }
    else
    {
        std::cerr << "Error: No texture attached for the specified attachment type." << std::endl;
    }
}

unsigned int FrameBuffer::GetTextureID(AttachmentType type) const
{
    auto it = m_Textures.find(type);
    if (it != m_Textures.end())
    {
        return it->second;
    }
    else
    {
        std::cerr << "Error: No texture attached for the specified attachment type." << std::endl;
        return 0;
    }
}

void FrameBuffer::CheckFramebufferComplete() const
{
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Error: Framebuffer is not complete!" << std::endl;
    }
}
