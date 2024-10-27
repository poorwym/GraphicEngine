#pragma once
#include "Renderer.h"
#include <unordered_map>

class FrameBuffer
{
public:
    enum AttachmentType
    {
        COLOR_ATTACHMENT,
        DEPTH_ATTACHMENT,
        // 可以根据需要添加更多附件类型
    };

    FrameBuffer(unsigned int width, unsigned int height);
    ~FrameBuffer();

    void Bind() const;
    void Unbind() const;

    void BindTexture(unsigned int slot, AttachmentType type = DEPTH_ATTACHMENT) const;

    unsigned int GetTextureID(AttachmentType type = DEPTH_ATTACHMENT) const;

private:
    unsigned int m_RendererID;
    unsigned int m_Width;
    unsigned int m_Height;

    std::unordered_map<AttachmentType, unsigned int> m_Textures;
    unsigned int m_RBO;

    void CheckFramebufferComplete() const;
};
