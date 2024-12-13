#pragma once


#include "TinyOBJLoader/tiny_obj_loader.h"
#include "Component.h"
#include "Material.h"

class OBJManager
{
public:
    OBJManager();
    ~OBJManager();
    MeshComponent* Load(const std::string& filePath, const std::string fileName, float scaleRate);
private:
    PBRMaterial * ProcessMaterial(const std::string& filePath, const tinyobj::material_t & m);
};