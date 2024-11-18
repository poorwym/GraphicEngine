#pragma once
#include "Renderer.h"
#include <unordered_map>
#include "Texture.h"

class FrameBuffer
{
public:
    FrameBuffer(unsigned int width, unsigned int height);
    virtual ~FrameBuffer();

    void Bind() const;
    void Unbind() const;

    virtual void BindTexture(unsigned int slot) const {};


protected:
    unsigned int m_RendererID;
    unsigned int m_Width;
    unsigned int m_Height;
    void CheckFramebufferComplete() const;
};

class DepthMapFBO : public FrameBuffer {
public:
    DepthMapFBO(unsigned int width, unsigned int height);
    ~DepthMapFBO();
    void BindTexture(unsigned int slot) const override;
private:
    unsigned int m_TextureID;
};

class CubeMapFBO : public FrameBuffer {
public:
    CubeMapFBO(unsigned int width, unsigned int height);
    ~CubeMapFBO();

    // ��ָ������������ͼ��������Ⱦ
    void BindFace(unsigned int face) const;

    // ����������ͼ����ɫ����ָ�������
    void BindTexture(unsigned int slot) const override;

    // ��ȡ��������ͼID
    unsigned int GetCubeMapTextureID() const { return m_TextureID; }

private:
    unsigned int m_TextureID;
};

// ��� ColorFBO ��
class ColorFBO : public FrameBuffer {
public:
    ColorFBO(unsigned int width, unsigned int height);
    ~ColorFBO();

    // ����ɫ����ָ�������
    void BindTexture(unsigned int slot) const override;
    void BindDepthTexture(unsigned int slot) const;

    // ��ȡ��ɫ����ID
    unsigned int GetColorTextureID() const { return m_ColorTextureID; }

private:
    unsigned int m_ColorTextureID;
    unsigned int m_DepthTextureID;
    bool m_HasDepth;
};