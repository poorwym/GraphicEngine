#include "ResourceManager.h"
#include <iostream>
#include <vector>
#include "Component.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"
#include <GL/glew.h>
#include <filesystem>
#ifndef TINYOBJLOADER_IMPLEMENTATION
    #define TINYOBJLOADER_IMPLEMENTATION
#endif 
#include "TinyOBJLoader/tiny_obj_loader.h"

static void EnsureDirectoryExists(const std::string& directory) {
    std::filesystem::path dirPath(directory);
    if (!std::filesystem::exists(dirPath)) {
        std::filesystem::create_directories(dirPath);
    }
}

MeshComponent* ResourceManager::LoadOBJ(const std::string& filePath, const std::string fileName, float scaleRate = 1)
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

        Mesh* mesh = new Mesh(vertices, indices, loadedMaterials[materialID], scaleRate);
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

void ResourceManager::SaveFBOToPNG(ColorFBO& colorFBO, const std::string& filename, int width, int height)
{
    std::string directory = "output/images/";
    std::string fullPath = directory + filename;
    // ȷ��Ŀ��Ŀ¼����
    EnsureDirectoryExists(directory);

    // ��֡����
    colorFBO.Bind();

    // ����һ���������洢��������
    std::vector<unsigned char> pixels(width * height * 4); // ������ RGBA ��ʽ

    // ��ȡ֡�������ɫ����
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

    // �����
    colorFBO.Unbind();

    // OpenGL ������ԭ�������½ǣ���Ҫ��ͼ��ת
    for (int y = 0; y < height / 2; ++y) {
        for (int x = 0; x < width * 4; ++x) {
            std::swap(pixels[y * width * 4 + x], pixels[(height - 1 - y) * width * 4 + x]);
        }
    }

    // ʹ�� stb_image_write ���� PNG
    if (stbi_write_png(fullPath.c_str(), width, height, 4, pixels.data(), width * 4)) {
        std::cout << "Successfully saved FBO to " << fullPath << std::endl;
    }
    else {
        std::cerr << "Failed to save FBO to " << fullPath << std::endl;
    }
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

