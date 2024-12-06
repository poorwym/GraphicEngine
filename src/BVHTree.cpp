#include "BVHTree.h"

BVHTree::BVHTree(const std::vector<Triangle>& triangles) {
    // 初始化 triangleIndices
    triangleIndices.reserve(triangles.size());
    for (int i = 0; i < triangles.size(); ++i) {
        triangleIndices.push_back(i);
    }

    // 构建 BVH 树
    buildNode(0, triangleIndices.size(), triangles);
}

int BVHTree::buildNode(int start, int end, const std::vector<Triangle>& triangles) {
    BVHNode node;
    // 计算包围盒
    node.bounds = computeAABB(start, end, triangles);

    int nodeIndex = nodes.size();
    nodes.push_back(node);

    int count = end - start;
    if (count <= 4) { // 叶子节点阈值
        nodes[nodeIndex].left = -1;
        nodes[nodeIndex].right = -1;
        nodes[nodeIndex].start = start;
        nodes[nodeIndex].count = count;
        return nodeIndex;
    }

    // 按照某个轴排序（例如 x 轴）
    int axis = 0; // 0: x, 1: y, 2: z
    std::sort(triangleIndices.begin() + start, triangleIndices.begin() + end,
        [&](int a, int b) -> bool {
            float centroidA = (triangles[a].position[0].x + triangles[a].position[1].x + triangles[a].position[2].x) / 3.0f;
            float centroidB = (triangles[b].position[0].x + triangles[b].position[1].x + triangles[b].position[2].x) / 3.0f;
            return centroidA < centroidB;
        });

    int mid = start + count / 2;
    nodes[nodeIndex].left = buildNode(start, mid, triangles);
    nodes[nodeIndex].right = buildNode(mid, end, triangles);
    nodes[nodeIndex].start = -1;
    nodes[nodeIndex].count = 0;

    return nodeIndex;
}

AABB BVHTree::computeAABB(int start, int end, const std::vector<Triangle>& triangles) {
    AABB aabb;
    aabb.min = glm::vec3(std::numeric_limits<float>::max());
    aabb.max = glm::vec3(std::numeric_limits<float>::lowest());

    for (int i = start; i < end; ++i) {
        const Triangle& tri = triangles[triangleIndices[i]];
        for (int j = 0; j < 3; ++j) {
            aabb.min = glm::min(aabb.min, glm::vec3(tri.position[j]));
            aabb.max = glm::max(aabb.max, glm::vec3(tri.position[j]));
        }
    }

    return aabb;
}
