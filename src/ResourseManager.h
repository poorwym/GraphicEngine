#pragma once
#include <string>

class ResourceManager {
public:
    template<typename T>
    T* Load(const std::string& filePath);         // 从文件加载资源

    template<typename T>
    void Unload(T* resource);                     // 卸载资源
};