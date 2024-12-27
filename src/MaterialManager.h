#pragma once
#include "Material.h"

class MaterialEditor {
private:
    PBRMaterial& m_Material;
public:
	MaterialEditor(PBRMaterial & material);
	void OnImGuiRender();
};

class MaterialManager {
public:
	void AddMaterial(PBRMaterial& material);
	int GetMaterialIndex(const PBRMaterial & material);
	void OnImGuiRender();
};