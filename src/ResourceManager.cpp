#include "ResourceManager.h"
#include <iostream>
#include <vector>
#include "Component.h"
#include <GL/glew.h>
#include <filesystem>
#include "OBJManager.h"
#include "GLTFManager.h"

static void EnsureDirectoryExists(const std::string& directory) {
    std::filesystem::path dirPath(directory);
    if (!std::filesystem::exists(dirPath)) {
        std::filesystem::create_directories(dirPath);
    }
}

MeshComponent* ResourceManager::LoadOBJ(const std::string& filePath, const std::string fileName, float scaleRate = 1)
{
   OBJManager objManager;
   return objManager.Load(filePath, fileName, scaleRate);
}

SceneNode* ResourceManager::LoadGLTF(const std::string& filePath, const std::string& fileName, float scaleRate)
{
    GLTFManager gltfManager;
    return gltfManager.Load(filePath, fileName, scaleRate);
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

template<>
ComputeShader* ResourceManager::Load<ComputeShader>(const std::string& filePath)
{
    ComputeShader* computeShader = new ComputeShader(filePath);
    return computeShader;
}
/*
MeshComponent* ResourceManager::LoadFBXMesh(FbxMesh* fbxMesh, const std::string& sceneName, const std::string& filePath)
{
    // 获取顶点数量
    int vertexCount = fbxMesh->GetControlPointsCount();
    FbxVector4* controlPoints = fbxMesh->GetControlPoints();

    // 获取面数量
    int polygonCount = fbxMesh->GetPolygonCount();

    // 准备存储顶点和索引
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // 假设你正在遍历 FBX 模型的多边形和顶点
    for (int i = 0; i < polygonCount; ++i) {
        int verticesPerPolygon = fbxMesh->GetPolygonSize(i);
        if (verticesPerPolygon != 3) {
            // 仅处理三角形
            continue;
        }

        for (int j = 0; j < verticesPerPolygon; ++j) {
            int controlPointIndex = fbxMesh->GetPolygonVertex(i, j);
            FbxVector4 fbxPosition = controlPoints[controlPointIndex];
            Vertex vertex;
            vertex.Position = glm::vec4(
                static_cast<float>(fbxPosition[0]),
                static_cast<float>(fbxPosition[1]),
                static_cast<float>(fbxPosition[2]),
                1.0f
            );

            // 获取法线
            FbxVector4 fbxNormal;
            bool normalExists = fbxMesh->GetPolygonVertexNormal(i, j, fbxNormal);
            if (normalExists) {
                vertex.Normal = glm::vec4(
                    static_cast<float>(fbxNormal[0]),
                    static_cast<float>(fbxNormal[1]),
                    static_cast<float>(fbxNormal[2]),
                    1.0f
                );
            }
            else {
                vertex.Normal = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            }

            // 获取 UV 坐标
            FbxVector2 fbxUV;
            bool unmapped = false;
            bool uvExists = fbxMesh->GetPolygonVertexUV(i, j, "UVSet1", fbxUV, unmapped);
            if (uvExists && !unmapped) {
                vertex.TexCoords = glm::vec4(
                    static_cast<float>(fbxUV[0]),
                    static_cast<float>(fbxUV[1]),
                    0.0f,
                    0.0f
                );
            }
            else {
                // 处理未映射的 UV 坐标
                vertex.TexCoords = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
                if (unmapped) {
                    // 例如，记录日志或分配默认 UV
                    std::cerr << "Unmapped UV detected at polygon " << i << ", vertex " << j << std::endl;
                }
            }

            // 添加顶点和索引
            vertices.push_back(vertex);
            indices.push_back(static_cast<unsigned int>(indices.size()));
        }
    }

    // 创建MeshComponent
    MeshComponent* meshComponent = new MeshComponent();

    // 处理材质
    int materialCount = fbxMesh->GetElementMaterialCount();
    if (materialCount > 0) {
        // 假设每个网格只有一个材质
        FbxSurfaceMaterial* fbxMaterial = fbxMesh->GetNode()->GetMaterial(0);
        if (fbxMaterial) {
            PBRMaterial* pbrMaterial = LoadPBRMaterial(fbxMaterial, filePath);
            if (pbrMaterial) {
                // 创建Mesh并添加到MeshComponent
                Mesh* mesh = new Mesh(vertices, indices, pbrMaterial, 1.0f);
                meshComponent->AddMesh(mesh);
            }
        }
    }
    else {
        // 如果没有材质，可以使用默认材质
        PBRMaterial* defaultMaterial = new PBRMaterial();
        Mesh* mesh = new Mesh(vertices, indices, defaultMaterial, 1.0f);
        meshComponent->AddMesh(mesh);
    }

    return meshComponent;
}

// 加载FBX材质并返回PBRMaterial
PBRMaterial* ResourceManager::LoadPBRMaterial(FbxSurfaceMaterial* fbxMaterial, const std::string& filePath)
{
    PBRMaterial* pbrMaterial = new PBRMaterial(filePath, fbxMaterial);
    return pbrMaterial;
}
*/