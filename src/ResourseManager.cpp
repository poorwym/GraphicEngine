#include "ResourseManager.h"
#include <iostream>
#include <vector>
#ifndef TINYOBJLOADER_IMPLEMENTATION
    #define TINYOBJLOADER_IMPLEMENTATION
#endif 
#include "TinyOBJLoader/tiny_obj_loader.h"

Mesh* ResourceManager::LoadOBJ(const std::string& filePath, const std::string fileName)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, (filePath+fileName).c_str(), nullptr, true)) {
        std::cerr <<  err << std::endl;
        return nullptr;
    }

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex = {};
            vertex.Position = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };
            vertex.Normal = {
                attrib.normals[3 * index.normal_index + 0],
                attrib.normals[3 * index.normal_index + 1],
                attrib.normals[3 * index.normal_index + 2]
            };
            vertex.TexCoords = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                attrib.texcoords[2 * index.texcoord_index + 1]
            };
            vertices.push_back(vertex);
            indices.push_back(indices.size());
        }
    }
    
    PBRMaterial* material = LoadPBRMaterial(filePath);
    Mesh* mesh = new Mesh(vertices, indices, material);
    return mesh;
}

PBRMaterial* ResourceManager::LoadPBRMaterial(const std::string& filePath)
{
    PBRMaterial* material = new PBRMaterial();
    Texture* albedo = new Texture(filePath+"col.png");// 漫反射贴图
    if (albedo) material->SetAlbedoMap(albedo);
    Texture* normal = new Texture(filePath+"nrm.png");// 法线贴图
    if (normal) material->SetNormalMap(normal);
    Texture* metallic = new Texture(filePath+"met.png");//金属度贴图
    if (metallic) material->SetMetallicMap(metallic);
    Texture* roughness = new Texture(filePath+"roughness.png");//粗糙度贴图
    if(roughness) material->SetRoughnessMap(roughness);
    Texture* ao = new Texture(filePath+"ao.png");//环境光遮蔽贴图
    if (ao) material->SetAO(ao);
    Texture* height = new Texture(filePath+"height.png");//高度贴图
    if (height) material->SetHeightMap(height);
    Texture* emision = new Texture(filePath+"emision.png");//自发光贴图
    if (emision) material->SetEmisionMap(emision);
    return material;
}

template<>
Shader* ResourceManager::Load<Shader>(const std::string& filePath)
{
    Shader* shader = new Shader(filePath);
    return shader;
}

template<>
Texture* ResourceManager::Load<Texture>(const std::string& filePath)
{
    Texture* texture = new Texture(filePath);
    return texture;
}

