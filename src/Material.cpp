#include "Material.h"
#include <iostream>
#include <string>
#include "SceneManager.h"
#include "TextureManager.h"

extern TextureManager g_TextureManager;
extern SceneManager g_SceneManager;
extern std::vector<Material> g_MaterialList;

// 默认构造函数
PBRMaterial::PBRMaterial()
    : m_Material(default_material)
{
}

void PBRMaterial::SetMaterial(const Material& material)
{
    m_Material = material;
}

PBRMaterial::~PBRMaterial()
{
}
