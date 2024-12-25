#include "TriangleSubdivider.h"
#include <glm/glm.hpp>
#include "SceneManager.h"

void TriangleSubdivider::SubdivideTriangle(
    const Triangle& inputTriangle,
    int subdivisionLevel,
    std::vector<Triangle>& outputTriangles
) {
    Subdivide(inputTriangle, subdivisionLevel, outputTriangles);
}

extern std::vector<Material> g_MaterialList;
extern SceneManager g_SceneManager;

Triangle TriangleSubdivider::CreateTriangle(Vertex& v1, Vertex& v2, Vertex& v3)
{
    Triangle triangle;
    triangle.position[0] = v1.Position;
    triangle.position[1] = v2.Position;
    triangle.position[2] = v3.Position;
    triangle.normal[0] = v1.Normal, 0.0f;
    triangle.normal[1] = v2.Normal, 0.0f;
    triangle.normal[2] = v3.Normal, 0.0f;
    triangle.texCoords[0] = v1.TexCoords, 0.0f, 0.0f;
    triangle.texCoords[1] = v2.TexCoords, 0.0f, 0.0f;
    triangle.texCoords[2] = v3.TexCoords, 0.0f, 0.0f;
    triangle.tangent[0] = v1.Tangent, 0.0f;
    triangle.tangent[1] = v2.Tangent, 0.0f;
    triangle.tangent[2] = v3.Tangent, 0.0f;
    triangle.bitangent[0] = v1.Bitangent;
    triangle.bitangent[1] = v2.Bitangent;
    triangle.bitangent[2] = v3.Bitangent;
    
    triangle.materialIndex = v1.MaterialIndex.x;
    return triangle;
}

void TriangleSubdivider::AdjustHeight(Triangle& triangle, CPUTexture* heightMap, float bumpMutiplier)
{
    if (heightMap == nullptr)
    {
        std::cerr << "Error: Height map is null." << std::endl;
        return;
    }

    // Loop over each vertex in the triangle
    for (int i = 0; i < 3; ++i)
    {
        // Extract the vertex attributes
        glm::vec3 position = glm::vec3(triangle.position[i]);
        glm::vec3 normal = glm::vec3(triangle.normal[i]);
        glm::vec2 texCoords = glm::vec2(triangle.texCoords[i]);

        // Ensure the texture coordinates are in [0,1] range
        texCoords = glm::fract(texCoords);

        // Sample the height map at the vertex's texture coordinates
        cv::Vec2f texCoordCV(texCoords.x, texCoords.y);
        float height = heightMap->GetGrayValue(texCoordCV);

        // Adjust the height value if necessary
        // Assuming height map values are in [0,1], and we want displacement in [-0.5, 0.5]
        float displacement = (height - 0.5f) * 2.0f * bumpMutiplier;

        // Displace the vertex along its normal
        position += normal * displacement;

        // Update the vertex position in the triangle
        triangle.position[i] = glm::vec4(position, triangle.position[i].w);
    }
}

void TriangleSubdivider::Subdivide(
    const Triangle& triangle,
    int level,
    std::vector<Triangle>& triangles
) {
    if (level == 0) {
        // 递归终止条件：将当前三角形添加到结果列表中
        triangles.push_back(triangle);
        return;
    }

    // 提取三角形的三个顶点
    Vertex v0, v1, v2;
    //v0 = { triangle.position[0], triangle.normal[0], triangle.texCoords[0], triangle.tangent[0], triangle.bitangent[0], triangle.material };
    //v1 = { triangle.position[1], triangle.normal[1], triangle.texCoords[1], triangle.tangent[1], triangle.bitangent[1], triangle.material };
    //v2 = { triangle.position[2], triangle.normal[2], triangle.texCoords[2], triangle.tangent[2], triangle.bitangent[2], triangle.material };

    // 计算三条边的中点
    Vertex m0 = ComputeMidpoint(v0, v1);
    Vertex m1 = ComputeMidpoint(v1, v2);
    Vertex m2 = ComputeMidpoint(v2, v0);

    // 创建四个新的三角形
    Triangle t0, t1, t2, t3;

    // 三角形 t0
    t0.position[0] = v0.Position;
    t0.position[1] = m0.Position;
    t0.position[2] = m2.Position;

    t0.normal[0] = v0.Normal;
    t0.normal[1] = m0.Normal;
    t0.normal[2] = m2.Normal;

    t0.texCoords[0] = v0.TexCoords;
    t0.texCoords[1] = m0.TexCoords;
    t0.texCoords[2] = m2.TexCoords;

    t0.tangent[0] = v0.Tangent;
    t0.tangent[1] = m0.Tangent;
    t0.tangent[2] = m2.Tangent;

    t0.bitangent[0] = v0.Bitangent;
    t0.bitangent[1] = m0.Bitangent;
    t0.bitangent[2] = m2.Bitangent;

    //t0.material = triangle.material;

    // 三角形 t1
    t1.position[0] = m0.Position;
    t1.position[1] = v1.Position;
    t1.position[2] = m1.Position;

    t1.normal[0] = m0.Normal;
    t1.normal[1] = v1.Normal;
    t1.normal[2] = m1.Normal;

    t1.texCoords[0] = m0.TexCoords;
    t1.texCoords[1] = v1.TexCoords;
    t1.texCoords[2] = m1.TexCoords;

    t1.tangent[0] = m0.Tangent;
    t1.tangent[1] = v1.Tangent;
    t1.tangent[2] = m1.Tangent;

    t1.bitangent[0] = m0.Bitangent;
    t1.bitangent[1] = v1.Bitangent;
    t1.bitangent[2] = m1.Bitangent;

    //t1.material = triangle.material;

    // 三角形 t2
    t2.position[0] = m2.Position;
    t2.position[1] = m1.Position;
    t2.position[2] = v2.Position;

    t2.normal[0] = m2.Normal;
    t2.normal[1] = m1.Normal;
    t2.normal[2] = v2.Normal;

    t2.texCoords[0] = m2.TexCoords;
    t2.texCoords[1] = m1.TexCoords;
    t2.texCoords[2] = v2.TexCoords;

    t2.tangent[0] = m2.Tangent;
    t2.tangent[1] = m1.Tangent;
    t2.tangent[2] = v2.Tangent;

    t2.bitangent[0] = m2.Bitangent;
    t2.bitangent[1] = m1.Bitangent;
    t2.bitangent[2] = v2.Bitangent;

    //t2.material = triangle.material;

    // 三角形 t3
    t3.position[0] = m0.Position;
    t3.position[1] = m1.Position;
    t3.position[2] = m2.Position;

    t3.normal[0] = m0.Normal;
    t3.normal[1] = m1.Normal;
    t3.normal[2] = m2.Normal;

    t3.texCoords[0] = m0.TexCoords;
    t3.texCoords[1] = m1.TexCoords;
    t3.texCoords[2] = m2.TexCoords;

    t3.tangent[0] = m0.Tangent, 0.0f;
    t3.tangent[1] = m1.Tangent, 0.0f;
    t3.tangent[2] = m2.Tangent, 0.0f;

    t3.bitangent[0] = m0.Bitangent;
    t3.bitangent[1] = m1.Bitangent;
    t3.bitangent[2] = m2.Bitangent;

    //t3.material = triangle.material;

    // 递归细分新生成的三角形
    Subdivide(t0, level - 1, triangles);
    Subdivide(t1, level - 1, triangles);
    Subdivide(t2, level - 1, triangles);
    Subdivide(t3, level - 1, triangles);
}

Vertex TriangleSubdivider::ComputeMidpoint(const Vertex& v1, const Vertex& v2) {
    Vertex midpoint;
    midpoint.Position = 0.5f * (v1.Position + v2.Position);
    midpoint.Normal = glm::normalize(0.5f * (v1.Normal + v2.Normal));
    midpoint.TexCoords = 0.5f * (v1.TexCoords + v2.TexCoords);
    midpoint.Tangent = glm::normalize(0.5f * (v1.Tangent + v2.Tangent));
    midpoint.Bitangent = glm::normalize(0.5f * (v1.Bitangent + v2.Bitangent));
    midpoint.MaterialIndex = v1.MaterialIndex; // 假设材质相同
    return midpoint;
}