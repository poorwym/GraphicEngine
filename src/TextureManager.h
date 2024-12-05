#pragma once
#include "Texture.h"
#include <vector>

class TextureManager {
public:
    TextureManager();
    ~TextureManager();
    // 添加纹理，返回其索引
    uint32_t AddTexture(CPUTexture* texture);

    // 根据索引获取纹理
    CPUTexture* GetTexture(uint32_t index) const;


private:
    std::vector<CPUTexture*> textures;
};
