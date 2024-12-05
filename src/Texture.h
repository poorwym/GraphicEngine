#pragma once
#include <string>
#include "Renderer.h"
#include <glm/glm.hpp>
#include "opencv2/opencv.hpp"

class Texture
{
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;
public:
	Texture(const std::string& path);
	~Texture();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
	inline unsigned int GetTextureID() const { return m_RendererID; }
};

class CPUTexture {
public:
    CPUTexture();
    CPUTexture(const std::string& filename);

    // 加载纹理
    bool LoadTexture(const std::string& filename);

    // 获取纹理尺寸
    int GetWidth() const;
    int GetHeight() const;

    // 采样纹理颜色，纹理坐标范围为 [0,1]
    cv::Vec3f Sample(const cv::Vec2f& texCoord) const;

    // 获取灰度值（用于高度贴图或置换贴图）
    float GetGrayValue(const cv::Vec2f& texCoord) const;

    // 获取纹理通道数
    int GetChannelCount() const;

private:
    cv::Mat textureImage; // 存储纹理图像
};