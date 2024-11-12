#include "ResourceManager.h"
#include <iostream>
#include <vector>
#include "Component.h"
#ifndef TINYOBJLOADER_IMPLEMENTATION
    #define TINYOBJLOADER_IMPLEMENTATION
#endif 
#include "TinyOBJLoader/tiny_obj_loader.h"

MeshComponent* ResourceManager::LoadOBJ(const std::string& filePath, const std::string fileName)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, (filePath + fileName).c_str(), filePath.c_str(), true)) {
        std::cerr << "TinyOBJLoader Error: " << err << std::endl;
        return nullptr;
    }
    std::cout << "shapes = " << shapes.size() << std::endl;
    std::cout << "materials= " << materials.size() << std::endl;

    // �������в���
    std::vector<PBRMaterial*> loadedMaterials;
    for (const auto& mat : materials) {
        PBRMaterial* pbrMat = new PBRMaterial(filePath, mat);
        loadedMaterials.push_back(pbrMat);
    }

    std::map<int, std::vector<Vertex>> materialVerticesMap;
    std::map<int, std::vector<unsigned int>> materialIndicesMap;

    // ����ÿ����״
    for (const auto& shape : shapes) {
        size_t indexOffset = 0;
        // ����ÿ����
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            size_t fv = shape.mesh.num_face_vertices[f];

            int materialID = shape.mesh.material_ids[f];
            if (materialID < 0 || materialID >= materials.size()) {
                materialID = 0; // ʹ��Ĭ�ϲ���
            }

            // ȷ��ӳ���д��ڸò��ʵ���Ŀ
            if (materialVerticesMap.find(materialID) == materialVerticesMap.end()) {
                materialVerticesMap[materialID] = std::vector<Vertex>();
                materialIndicesMap[materialID] = std::vector<unsigned int>();
            }

            // �������е�ÿ������
            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shape.mesh.indices[indexOffset + v];
                Vertex vertex = {};
                vertex.Position = {
                    attrib.vertices[3 * idx.vertex_index + 0],
                    attrib.vertices[3 * idx.vertex_index + 1],
                    attrib.vertices[3 * idx.vertex_index + 2]
                };
                if (idx.normal_index >= 0) {
                    vertex.Normal = {
                        attrib.normals[3 * idx.normal_index + 0],
                        attrib.normals[3 * idx.normal_index + 1],
                        attrib.normals[3 * idx.normal_index + 2]
                    };
                }
                if (idx.texcoord_index >= 0) {
                    vertex.TexCoords = {
                        attrib.texcoords[2 * idx.texcoord_index + 0],
                        attrib.texcoords[2 * idx.texcoord_index + 1]
                    };
                }

                // ��Ӷ��㲢��������
                materialVerticesMap[materialID].push_back(vertex);
                materialIndicesMap[materialID].push_back(materialVerticesMap[materialID].size() - 1);
            }

            indexOffset += fv;
        }
    }

    // ���� MeshComponent ����Ӻϲ���� Mesh
    MeshComponent* meshComponent = new MeshComponent();
    for (const auto& pair : materialVerticesMap) {
        int materialID = pair.first;
        std::vector<Vertex>& vertices = materialVerticesMap[materialID];
        std::vector<unsigned int>& indices = materialIndicesMap[materialID];

        Mesh* mesh = new Mesh(vertices, indices, loadedMaterials[materialID]);
        meshComponent->AddMesh(mesh);
    }

    return meshComponent;
}


PBRMaterial* ResourceManager::LoadPBRMaterial(const std::string& filePath)
{
    PBRMaterial* material = new PBRMaterial();
    Texture* albedo = new Texture(filePath+"col.png");// ��������ͼ
    if (albedo) material->SetAlbedoMap(albedo);
    Texture* normal = new Texture(filePath+"nrm.png");// ������ͼ
    if (normal) material->SetNormalMap(normal);
    Texture* metallic = new Texture(filePath+"met.png");//��������ͼ
    if (metallic) material->SetMetallicMap(metallic);
    Texture* roughness = new Texture(filePath+"roughness.png");//�ֲڶ���ͼ
    if(roughness) material->SetRoughnessMap(roughness);
    Texture* ao = new Texture(filePath+"ao.png");//�������ڱ���ͼ
    if (ao) material->SetAO(ao);
    Texture* height = new Texture(filePath+"height.png");//�߶���ͼ
    if (height) material->SetHeightMap(height);
    Texture* emision = new Texture(filePath+"emision.png");//�Է�����ͼ
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

