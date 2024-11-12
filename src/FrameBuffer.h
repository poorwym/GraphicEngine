#pragma once
#include "Renderer.h"
#include <unordered_map>
#include "Texture.h"

class FrameBuffer
{
public:
    FrameBuffer(unsigned int width, unsigned int height);
    ~FrameBuffer();

    void Bind() const;
    void Unbind() const;

    virtual void BindTexture(unsigned int slot) const {};

    unsigned int GetTextureID() const;

protected:
    unsigned int m_RendererID;
    unsigned int m_Width;
    unsigned int m_Height;
    void CheckFramebufferComplete() const;
};

class DepthMapFBO : public FrameBuffer {
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

    // 绑定指定的立方体贴图面用于渲染
    void BindFace(unsigned int face) const;

    // 绑定立方体贴图到着色器的指定纹理槽
    void BindTexture(unsigned int slot) const override;

    // 获取立方体贴图ID
    unsigned int GetCubeMapTextureID() const { return m_TextureID; }

private:
    unsigned int m_TextureID;
};