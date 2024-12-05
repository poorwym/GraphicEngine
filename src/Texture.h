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

    // ��������
    bool LoadTexture(const std::string& filename);

    // ��ȡ����ߴ�
    int GetWidth() const;
    int GetHeight() const;

    // ����������ɫ���������귶ΧΪ [0,1]
    cv::Vec3f Sample(const cv::Vec2f& texCoord) const;

    // ��ȡ�Ҷ�ֵ�����ڸ߶���ͼ���û���ͼ��
    float GetGrayValue(const cv::Vec2f& texCoord) const;

    // ��ȡ����ͨ����
    int GetChannelCount() const;

private:
    cv::Mat textureImage; // �洢����ͼ��
};