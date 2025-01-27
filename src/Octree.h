#pragma once
#include "Mesh.h"
#include <algorithm>
#include <limits>
#include "AABB.h"

// 若children[i] = -1则表示无该子节点
struct OctreeNode {
    AABB bounds;
    int children[8]; // 8个子节点索引
    int start;  // 叶子节点中的三角形起始索引
    int count;  // 叶子节点中的三角形数量
    float pad3;
    float pad4;
};

class Octree {
public:
    std::vector<OctreeNode> nodes;
    std::vector<int> triangleIndices; // 存储三角形的索引

    Octree(const std::vector<Triangle>& triangles, int maxDepth = 10, int leafThreshold = 4);

private:
    int buildNode(int start, int end, const std::vector<Triangle>& triangles, int depth, int maxDepth, int leafThreshold);
    AABB computeAABB(int start, int end, const std::vector<Triangle>& triangles);
    void subdivideAABB(const AABB& parentBounds, AABB childBounds[8]);
};