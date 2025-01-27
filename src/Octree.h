#pragma once
#include "Mesh.h"
#include <algorithm>
#include <limits>
#include "AABB.h"

// ��children[i] = -1���ʾ�޸��ӽڵ�
struct OctreeNode {
    AABB bounds;
    int children[8]; // 8���ӽڵ�����
    int start;  // Ҷ�ӽڵ��е���������ʼ����
    int count;  // Ҷ�ӽڵ��е�����������
    float pad3;
    float pad4;
};

class Octree {
public:
    std::vector<OctreeNode> nodes;
    std::vector<int> triangleIndices; // �洢�����ε�����

    Octree(const std::vector<Triangle>& triangles, int maxDepth = 10, int leafThreshold = 4);

private:
    int buildNode(int start, int end, const std::vector<Triangle>& triangles, int depth, int maxDepth, int leafThreshold);
    AABB computeAABB(int start, int end, const std::vector<Triangle>& triangles);
    void subdivideAABB(const AABB& parentBounds, AABB childBounds[8]);
};