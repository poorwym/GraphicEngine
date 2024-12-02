#include "ResourceManager.h"
#include <iostream>
#include <vector>
#include "Component.h"
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

    // 加载所有材质
    std::vector<PBRMaterial*> loadedMaterials;
    for (const auto& mat : materials) {
        PBRMaterial* pbrMat = new PBRMaterial(filePath, mat);
        loadedMaterials.push_back(pbrMat);
    }

    std::map<int, std::vector<Vertex>> materialVerticesMap;
    std::map<int, std::vector<unsigned int>> materialIndicesMap;

    // 遍历每个形状
    for (const auto& shape : shapes) {
        size_t indexOffset = 0;
        // 遍历每个面
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            size_t fv = shape.mesh.num_face_vertices[f];

            int materialID = shape.mesh.material_ids[f];
            if (materialID < 0 || materialID >= materials.size()) {
                materialID = 0; // 使用默认材质
            }

            // 确保映射中存在该材质的条目
            if (materialVerticesMap.find(materialID) == materialVerticesMap.end()) {
                materialVerticesMap[materialID] = std::vector<Vertex>();
                materialIndicesMap[materialID] = std::vector<unsigned int>();
            }

            // 遍历面中的每个顶点
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

                // 添加顶点并更新索引
                materialVerticesMap[materialID].push_back(vertex);
                materialIndicesMap[materialID].push_back(materialVerticesMap[materialID].size() - 1);
            }
            indexOffset += fv;
        }
    }

    // 创建 MeshComponent 并添加合并后的 Mesh
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


void ResourceManager::SaveFBOToPNG(ColorFBO& colorFBO, const std::string& filename, int width, int height)
{
    std::string directory = "output/images/";
    std::string fullPath = directory + filename;
    // 确保目标目录存在
    EnsureDirectoryExists(directory);

    // 绑定帧缓冲
    colorFBO.Bind();

    // 创建一个数组来存储像素数据
    std::vector<unsigned char> pixels(width * height * 4); // 假设是 RGBA 格式

    // 读取帧缓冲的颜色附件
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

    // 解除绑定
    colorFBO.Unbind();

    // OpenGL 的像素原点在左下角，需要将图像翻转
    for (int y = 0; y < height / 2; ++y) {
        for (int x = 0; x < width * 4; ++x) {
            std::swap(pixels[y * width * 4 + x], pixels[(height - 1 - y) * width * 4 + x]);
        }
    }

    // 使用 OpenCV 保存 PNG 图片
    cv::Mat img(height, width, CV_8UC4, pixels.data()); // 使用 OpenCV 的矩阵类型
    if (cv::imwrite(fullPath, img)) {
        std::cout << "Successfully saved FBO to " << fullPath << std::endl;
    }
    else {
        std::cerr << "Failed to save FBO to " << fullPath << std::endl;
    }
}

cv::Mat ResourceManager::SaveFBOToMat(ColorFBO& colorFBO, int width, int height)
{
    // 绑定帧缓冲
    colorFBO.Bind();

    // 检查 FBO 是否完整
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is not complete: " << status << std::endl;
        colorFBO.Unbind();
        return cv::Mat(); // 返回空 Mat
    }

    // 创建一个数组来存储像素数据
    std::vector<unsigned char> pixels(width * height * 4); // 假设是 RGBA 格式

    // 设置像素对齐
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    // 读取帧缓冲的颜色附件
    GLCall(glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data()));

    // 检查 OpenGL 错误
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error during glReadPixels: " << error << std::endl;
        colorFBO.Unbind();
        return cv::Mat(); // 返回空 Mat
    }

    // 解除绑定
    colorFBO.Unbind();

    // OpenGL 的像素原点在左下角，需要将图像翻转
    for (int y = 0; y < height / 2; ++y) {
        for (int x = 0; x < width * 4; ++x) {
            std::swap(pixels[y * width * 4 + x], pixels[(height - 1 - y) * width * 4 + x]);
        }
    }

    // 创建 cv::Mat 并拷贝数据
    cv::Mat mat(height, width, CV_8UC4, pixels.data());

    // 使用 clone() 以确保 mat 拥有自己的数据
    cv::Mat matCopy = mat.clone();

    // 确认 matCopy 数据有效
    if (matCopy.empty()) {
        std::cerr << "Error: matCopy is empty after cloning." << std::endl;
    }
    else {
        std::cout << "SaveFBOToMat: Successfully created matCopy with "
            << matCopy.rows << " rows and " << matCopy.cols << " cols." << std::endl;
    }

    return matCopy;
}

void ResourceManager::SaveMatToPNG(cv::Mat& mat, const std::string& filename, int width, int height)
{
    // 定义输出目录
    std::string directory = "output/images/";
    std::string fullPath = directory + filename;

    // 检查并调整图像尺寸（如果需要）
    cv::Mat resizedMat;
    if (width > 0 && height > 0) {
        cv::resize(mat, resizedMat, cv::Size(width, height), 0, 0, cv::INTER_LINEAR);
    }
    else {
        resizedMat = mat;
    }

    // 确保图像具有 4 个通道（RGBA）
    cv::Mat rgbaMat;
    if (resizedMat.channels() == 1) {
        // 灰度图转换为 BGRA
        cv::cvtColor(resizedMat, rgbaMat, cv::COLOR_GRAY2BGRA);
    }
    else if (resizedMat.channels() == 3) {
        // BGR 转换为 BGRA
        cv::cvtColor(resizedMat, rgbaMat, cv::COLOR_BGR2BGRA);
    }
    else if (resizedMat.channels() == 4) {
        rgbaMat = resizedMat.clone();
    }
    else {
        std::cerr << "Unsupported number of channels: " << resizedMat.channels() << std::endl;
        return;
    }

    // 将 BGRA 转换为 RGBA
    cv::Mat rgbaFinal;
    cv::cvtColor(rgbaMat, rgbaFinal, cv::COLOR_BGRA2RGBA);

    // 保存为 PNG 文件
    if (cv::imwrite(fullPath, rgbaFinal)) {
        std::cout << "Successfully saved Mat to " << fullPath << std::endl;
    }
    else {
        std::cerr << "Failed to save Mat to " << fullPath << std::endl;
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
