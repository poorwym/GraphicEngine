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

    // ʹ�� OpenCV ���� PNG ͼƬ
    cv::Mat img(height, width, CV_8UC4, pixels.data()); // ʹ�� OpenCV �ľ�������
    if (cv::imwrite(fullPath, img)) {
        std::cout << "Successfully saved FBO to " << fullPath << std::endl;
    }
    else {
        std::cerr << "Failed to save FBO to " << fullPath << std::endl;
    }
}

cv::Mat ResourceManager::SaveFBOToMat(ColorFBO& colorFBO, int width, int height)
{
    // ��֡����
    colorFBO.Bind();

    // ��� FBO �Ƿ�����
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is not complete: " << status << std::endl;
        colorFBO.Unbind();
        return cv::Mat(); // ���ؿ� Mat
    }

    // ����һ���������洢��������
    std::vector<unsigned char> pixels(width * height * 4); // ������ RGBA ��ʽ

    // �������ض���
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    // ��ȡ֡�������ɫ����
    GLCall(glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data()));

    // ��� OpenGL ����
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error during glReadPixels: " << error << std::endl;
        colorFBO.Unbind();
        return cv::Mat(); // ���ؿ� Mat
    }

    // �����
    colorFBO.Unbind();

    // OpenGL ������ԭ�������½ǣ���Ҫ��ͼ��ת
    for (int y = 0; y < height / 2; ++y) {
        for (int x = 0; x < width * 4; ++x) {
            std::swap(pixels[y * width * 4 + x], pixels[(height - 1 - y) * width * 4 + x]);
        }
    }

    // ���� cv::Mat ����������
    cv::Mat mat(height, width, CV_8UC4, pixels.data());

    // ʹ�� clone() ��ȷ�� mat ӵ���Լ�������
    cv::Mat matCopy = mat.clone();

    // ȷ�� matCopy ������Ч
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
    // �������Ŀ¼
    std::string directory = "output/images/";
    std::string fullPath = directory + filename;

    // ��鲢����ͼ��ߴ磨�����Ҫ��
    cv::Mat resizedMat;
    if (width > 0 && height > 0) {
        cv::resize(mat, resizedMat, cv::Size(width, height), 0, 0, cv::INTER_LINEAR);
    }
    else {
        resizedMat = mat;
    }

    // ȷ��ͼ����� 4 ��ͨ����RGBA��
    cv::Mat rgbaMat;
    if (resizedMat.channels() == 1) {
        // �Ҷ�ͼת��Ϊ BGRA
        cv::cvtColor(resizedMat, rgbaMat, cv::COLOR_GRAY2BGRA);
    }
    else if (resizedMat.channels() == 3) {
        // BGR ת��Ϊ BGRA
        cv::cvtColor(resizedMat, rgbaMat, cv::COLOR_BGR2BGRA);
    }
    else if (resizedMat.channels() == 4) {
        rgbaMat = resizedMat.clone();
    }
    else {
        std::cerr << "Unsupported number of channels: " << resizedMat.channels() << std::endl;
        return;
    }

    // �� BGRA ת��Ϊ RGBA
    cv::Mat rgbaFinal;
    cv::cvtColor(rgbaMat, rgbaFinal, cv::COLOR_BGRA2RGBA);

    // ����Ϊ PNG �ļ�
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
    // ��ȡ��������
    int vertexCount = fbxMesh->GetControlPointsCount();
    FbxVector4* controlPoints = fbxMesh->GetControlPoints();

    // ��ȡ������
    int polygonCount = fbxMesh->GetPolygonCount();

    // ׼���洢���������
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // ���������ڱ��� FBX ģ�͵Ķ���κͶ���
    for (int i = 0; i < polygonCount; ++i) {
        int verticesPerPolygon = fbxMesh->GetPolygonSize(i);
        if (verticesPerPolygon != 3) {
            // ������������
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

            // ��ȡ����
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

            // ��ȡ UV ����
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
                // ����δӳ��� UV ����
                vertex.TexCoords = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
                if (unmapped) {
                    // ���磬��¼��־�����Ĭ�� UV
                    std::cerr << "Unmapped UV detected at polygon " << i << ", vertex " << j << std::endl;
                }
            }

            // ��Ӷ��������
            vertices.push_back(vertex);
            indices.push_back(static_cast<unsigned int>(indices.size()));
        }
    }

    // ����MeshComponent
    MeshComponent* meshComponent = new MeshComponent();

    // �������
    int materialCount = fbxMesh->GetElementMaterialCount();
    if (materialCount > 0) {
        // ����ÿ������ֻ��һ������
        FbxSurfaceMaterial* fbxMaterial = fbxMesh->GetNode()->GetMaterial(0);
        if (fbxMaterial) {
            PBRMaterial* pbrMaterial = LoadPBRMaterial(fbxMaterial, filePath);
            if (pbrMaterial) {
                // ����Mesh����ӵ�MeshComponent
                Mesh* mesh = new Mesh(vertices, indices, pbrMaterial, 1.0f);
                meshComponent->AddMesh(mesh);
            }
        }
    }
    else {
        // ���û�в��ʣ�����ʹ��Ĭ�ϲ���
        PBRMaterial* defaultMaterial = new PBRMaterial();
        Mesh* mesh = new Mesh(vertices, indices, defaultMaterial, 1.0f);
        meshComponent->AddMesh(mesh);
    }

    return meshComponent;
}

// ����FBX���ʲ�����PBRMaterial
PBRMaterial* ResourceManager::LoadPBRMaterial(FbxSurfaceMaterial* fbxMaterial, const std::string& filePath)
{
    PBRMaterial* pbrMaterial = new PBRMaterial(filePath, fbxMaterial);
    return pbrMaterial;
}
*/