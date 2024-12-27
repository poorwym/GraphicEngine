#include "Material.h"
#include <iostream>
#include <string>
#include "SceneManager.h"
#include "TextureManager.h"

extern TextureManager g_TextureManager;
extern SceneManager g_SceneManager;
extern std::vector<PBRMaterial> g_MaterialList;

PBRMaterial::PBRMaterial()
{
    m_Name = "default" + std::to_string(g_MaterialList.size());
    m_Material = default_material;
}

// 默认构造函数
PBRMaterial::PBRMaterial(const std::string& name)
    : m_Material(default_material)
{
    m_Name = name;
}

void PBRMaterial::SetMaterial(const Material& material)
{
    m_Material = material;
}

PBRMaterial::~PBRMaterial()
{
}
