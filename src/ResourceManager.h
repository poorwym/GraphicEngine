#pragma once
#include <string>
#include "Shader.h"
#include "Texture.h"
#include "Material.h"
#include "Component.h"

class ResourceManager {
public:
    template<typename T>
    T* Load(const std::string& filePath);         // ���ļ�������Դ
    MeshComponent* LoadOBJ(const std::string& filePath, const std::string fileName, float scaleRate);
    PBRMaterial* LoadPBRMaterial(const std::string& filePath);

    template<typename T>
    void Unload(T* resource);                     // ж����Դ

    void SaveFBOToPNG(ColorFBO& colorFBO, const std::string& filename, int width, int height);
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