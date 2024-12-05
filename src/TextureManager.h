#pragma once
#include "Texture.h"
#include <vector>

class TextureManager {
public:
    TextureManager();
    ~TextureManager();
    // �����������������
    uint32_t AddTexture(CPUTexture* texture);

    // ����������ȡ����
    CPUTexture* GetTexture(uint32_t index) const;


private:
    std::vector<CPUTexture*> textures;
};
