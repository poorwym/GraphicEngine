#pragma once
#include <string>

class ResourceManager {
public:
    template<typename T>
    T* Load(const std::string& filePath);         // ���ļ�������Դ

    template<typename T>
    void Unload(T* resource);                     // ж����Դ
};