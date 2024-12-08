#pragma once
#include "Mesh.h"
#include <algorithm>
#include "AABB.h"


struct BVHNode {
    AABB bounds;
    int left;   // 左子节点索引，-1 表示叶子节点
    int right;  // 右子节点索引，-1 表示叶子节点
    int start;  // 叶子节点中的三角形起始索引
    int count;  // 叶子节点中的三角形数量
    float pad3;
    float pad4;
    float pad5;
    float pad6;
};

class BVHTree {
public:
    std::vector<BVHNode> nodes;
    std::vector<int> triangleIndices; // 存储三角形的索引

    BVHTree(const std::vector<Triangle>& triangles);

private:
    int buildNode(int start, int end, const std::vector<Triangle>& triangles);
    AABB computeAABB(int start, int end, const std::vector<Triangle>& triangles);
};
