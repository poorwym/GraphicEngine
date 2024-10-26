#pragma once
#include <string>
#include "Shader.h"
#include "Texture.h"

class ResourceManager {
public:
    template<typename T>
    T* Load(const std::string& filePath);         // 从文件加载资源

    template<typename T>
    void Unload(T* resource);                     // 卸载资源
};

template<typename T>
T* ResourceManager::Load(const std::string& filePath)
{
    return nullptr;
}

template<>
Shader* ResourceManager::Load<Shader>(const std::string& filePath)
{
    Shader* shader = new Shader(filePath);
    return shader;
}

template<>
Texture* ResourceManager::Load<Texture>(const std::string& filePath)
{
    Texture* texture = new Texture(filePath);
    return texture;
}