#include "FrameBuffer.h"
#include <iostream>

FrameBuffer::FrameBuffer(unsigned int width, unsigned int height)
    : m_RendererID(0), m_Width(width), m_Height(height)
{
    // 生成帧缓冲
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

    // 创建深度纹理附件
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

    // 附加深度纹理到帧缓冲
    GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0));
    m_TextureID = depthTexture;

    // 不需要颜色缓冲
    GLCall(glDrawBuffer(GL_NONE));
    GLCall(glReadBuffer(GL_NONE));

    // 检查帧缓冲完整性
    CheckFramebufferComplete();

    // 解绑帧缓冲
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

DepthMapFBO::~DepthMapFBO()
{
    GLCall(glDeleteFramebuffers(1, &m_RendererID));
    GLCall(glDeleteTextures(1, &m_TextureID));
}

void DepthMapFBO::BindTexture(unsigned int slot) const
{
    GLCall(glActiveTexture(GL_TEXTURE0 + slot));//使用槽位slot
    GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureID));
}

// CubeMapFBO 构造函数
CubeMapFBO::CubeMapFBO(unsigned int width, unsigned int height)
    : FrameBuffer(width, height), m_TextureID(0)
{
    // 生成立方体贴图
    GLCall(glGenTextures(1, &m_TextureID));
    GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID));

    for (unsigned int i = 0; i < 6; ++i)
    {
        // 为每个面分配存储空间
        GLCall(glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT24,
            m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL
        ));
    }

    // 设置立方体贴图参数
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

    // 绑定帧缓冲对象
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));

    // 附加第一个立方体贴图面作为深度附件
    GLCall(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_TextureID, 0));

    // 不需要颜色缓冲
    GLCall(glDrawBuffer(GL_NONE));
    GLCall(glReadBuffer(GL_NONE));

    // 检查帧缓冲完整性
    CheckFramebufferComplete();

    // 解绑帧缓冲
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

// CubeMapFBO 析构函数
CubeMapFBO::~CubeMapFBO()
{
    GLCall(glDeleteFramebuffers(1, &m_RendererID));
    GLCall(glDeleteTextures(1, &m_TextureID));
}

// 绑定指定的立方体贴图面用于渲染
void CubeMapFBO::BindFace(unsigned int face) const
{
    if (face >= 6)
    {
        std::cerr << "Error: CubeMapFBO::BindFace - Face index out of range (0-5)" << std::endl;
        return;
    }

    // 绑定帧缓冲对象
    Bind();

    // 附加指定的立方体贴图面作为深度附件
    GLCall(glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
        m_TextureID, 0
    ));

    // 检查帧缓冲完整性
    CheckFramebufferComplete();
}

// 绑定立方体贴图到指定纹理槽
void CubeMapFBO::BindTexture(unsigned int slot) const
{
    GLCall(glActiveTexture(GL_TEXTURE0 + slot)); // 使用指定的纹理槽
    GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID));
}

ColorFBO::ColorFBO(unsigned int width, unsigned int height)
    : FrameBuffer(width, height), m_ColorTextureID(0), m_DepthTextureID(0)
{
    // 创建颜色纹理
    GLCall(glGenTextures(1, &m_ColorTextureID));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_ColorTextureID));
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    // 创建深度纹理
    GLCall(glGenTextures(1, &m_DepthTextureID));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_DepthTextureID));
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    // 将深度纹理附加到帧缓冲的深度附件点
    GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTextureID, 0));

    // 将颜色纹理附加到帧缓冲的颜色附件点
    GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorTextureID, 0));
    // 指定绘制缓冲区
    GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    GLCall(glDrawBuffers(1, DrawBuffers));

    // 检查帧缓冲完整性
    CheckFramebufferComplete();

    // 解绑帧缓冲
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

// ColorFBO 析构函数
ColorFBO::~ColorFBO()
{
    GLCall(glDeleteFramebuffers(1, &m_RendererID));
    GLCall(glDeleteTextures(1, &m_ColorTextureID));
    GLCall(glDeleteTextures(1, &m_DepthTextureID));

}

// 绑定颜色纹理到指定纹理槽
void ColorFBO::BindTexture(unsigned int slot) const
{
    GLCall(glActiveTexture(GL_TEXTURE0 + slot)); // 使用指定的纹理槽
    GLCall(glBindTexture(GL_TEXTURE_2D, m_ColorTextureID));
}

void ColorFBO::BindDepthTexture(unsigned int slot) const
{
    GLCall(glActiveTexture(GL_TEXTURE0 + slot)); // 使用指定的纹理槽
    GLCall(glBindTexture(GL_TEXTURE_2D, m_DepthTextureID));
}
