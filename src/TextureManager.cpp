// TextureManager.cpp
#include "TextureManager.h"

TextureManager::TextureManager()
{
}

TextureManager::~TextureManager()
{
    for (auto& texture : textures) {
        delete texture;
    }
}

uint32_t TextureManager::AddTexture(CPUTexture* texture) {
    textures.push_back(texture);
    return static_cast<uint32_t>(textures.size() - 1);
}

CPUTexture* TextureManager::GetTexture(uint32_t index) const {
    if (index < textures.size()) {
        return textures[index];
    }
    return nullptr;
}

TextureManager g_TextureManager;