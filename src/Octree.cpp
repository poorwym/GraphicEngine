#include "Octree.h"

Octree::Octree(const std::vector<Triangle>& triangles, int maxDepth, int leafThreshold) {
    // 初始化 triangleIndices
    triangleIndices.reserve(triangles.size());
    for (int i = 0; i < (int)triangles.size(); ++i) {
        triangleIndices.push_back(i);
    }

    // 构建 Octree
    buildNode(0, (int)triangleIndices.size(), triangles, 0, maxDepth, leafThreshold);
}

int Octree::buildNode(int start, int end, const std::vector<Triangle>& triangles, int depth, int maxDepth, int leafThreshold) {
    OctreeNode node;
    node.bounds = computeAABB(start, end, triangles);

    int nodeIndex = (int)nodes.size();
    nodes.push_back(node);

    int count = end - start;
    // 判断是否为叶子节点条件：数量小于等于阈值或者达到最大深度
    if (count <= leafThreshold || depth >= maxDepth) {
        for (int i = 0; i < 8; i++) {
            nodes[nodeIndex].children[i] = -1;
        }
        nodes[nodeIndex].start = start;
        nodes[nodeIndex].count = count;
        return nodeIndex;
    }

    // 计算八个子节点的AABB
    AABB childAABBs[8];
    subdivideAABB(nodes[nodeIndex].bounds, childAABBs);

    // 按子节点分类三角形
    std::vector<int> childTriangleIndices[8];
    childTriangleIndices->reserve(count);

    for (int i = start; i < end; ++i) {
        int triIdx = triangleIndices[i];
        const Triangle& tri = triangles[triIdx];
        // 计算三角形重心
        glm::vec3 centroid = (tri.position[0] + tri.position[1] + tri.position[2]) / 3.0f;

        // 判断重心落在哪个子节点中
        for (int c = 0; c < 8; ++c) {
            const AABB& cb = childAABBs[c];
            if (centroid.x >= cb.min.x && centroid.x <= cb.max.x &&
                centroid.y >= cb.min.y && centroid.y <= cb.max.y &&
                centroid.z >= cb.min.z && centroid.z <= cb.max.z) {
                childTriangleIndices[c].push_back(triangleIndices[i]);
                break;
            }
        }
    }

    // 写回triangleIndices的排序，以便连续的子区间构建子节点
    int currentStart = start;
    for (int c = 0; c < 8; ++c) {
        int childCount = (int)childTriangleIndices[c].size();
        // 无子三角形的子节点设为 -1
        if (childCount == 0) {
            nodes[nodeIndex].children[c] = -1;
            continue;
        }

        // 将子三角形拷回主数组
        for (int k = 0; k < childCount; ++k) {
            triangleIndices[currentStart + k] = childTriangleIndices[c][k];
        }

        // 构建子节点
        nodes[nodeIndex].children[c] = buildNode(currentStart, currentStart + childCount, triangles, depth + 1, maxDepth, leafThreshold);
        currentStart += childCount;
    }

    // 非叶子节点不需要start和count
    nodes[nodeIndex].start = -1;
    nodes[nodeIndex].count = 0;

    return nodeIndex;
}

AABB Octree::computeAABB(int start, int end, const std::vector<Triangle>& triangles) {
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

void Octree::subdivideAABB(const AABB& parentBounds, AABB childBounds[8]) {
    glm::vec3 center = (parentBounds.min + parentBounds.max) * 0.5f;
    glm::vec3 min = parentBounds.min;
    glm::vec3 max = parentBounds.max;

    // 八个子区域的AABB
    // 子立方体分布： 
    // index含义 (假设x:0左1右,y:0下1上,z:0后1前)
    // childIndex = (x_bit << 2) | (y_bit << 1) | z_bit
    // 例如 child 0: xyz全部是0，对应min到center
    // child 1: z=1, 其余0, 对应(min.x, min.y, center.z)到(center.x, center.y, max.z)
    // ... 以此类推
    for (int i = 0; i < 8; ++i) {
        int x = (i & 4) ? 1 : 0;
        int y = (i & 2) ? 1 : 0;
        int z = (i & 1) ? 1 : 0;

        glm::vec3 cmin = glm::vec3(x ? center.x : min.x,
            y ? center.y : min.y,
            z ? center.z : min.z);

        glm::vec3 cmax = glm::vec3(x ? max.x : center.x,
            y ? max.y : center.y,
            z ? max.z : center.z);

        childBounds[i].min = cmin;
        childBounds[i].max = cmax;
    }
}