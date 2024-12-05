#pragma once
#include <string>
#include "Shader.h"
#include "Texture.h"
#include "Material.h"
#include "Component.h"
#include <opencv2/opencv.hpp>
#include "ComputeShader.h"

class ResourceManager {
public:
    template<typename T>
    T* Load(const std::string& filePath);         // 从文件加载资源
    MeshComponent* LoadOBJ(const std::string& filePath, const std::string fileName, float scaleRate);
    PBRMaterial* LoadPBRMaterial(const std::string& filePath);

    template<typename T>
    void Unload(T* resource);                     // 卸载资源

    void SaveFBOToPNG(ColorFBO& colorFBO, const std::string& filename, int width, int height);
    cv::Mat SaveFBOToMat(ColorFBO& colorFBO, int width, int height);
    void SaveMatToPNG(cv::Mat& mat, const std::string& filename, int width, int height);
};

template<typename T>
T* ResourceManager::Load(const std::string& filePath)
{
    return nullptr;
}
template<>
Shader* ResourceManager::Load<Shader>(const std::string& filePath);

template<>
Texture* ResourceManager::Load<Texture>(const std::string& filePath);

template<>
ComputeShader* ResourceManager::Load<ComputeShader>(const std::string& filePath);