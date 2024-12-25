#pragma once
#include "Material.h"

class MaterialEditor {
private:
    Material& m_Material;
public:
	MaterialEditor(Material& material);
	void OnImGuiRender();
};

class MaterialManager {
public:
	void AddMaterial(Material& material, std::string name);
	void SetMaterialName(Material& material, std::string name);
	int GetMaterialIndex(const Material& material);
	void OnImGuiRender();
};