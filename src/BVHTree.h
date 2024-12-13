#pragma once
#include "Mesh.h"
#include <algorithm>
#include "AABB.h"


struct BVHNode {
    AABB bounds;
    int left;   // ���ӽڵ�������-1 ��ʾҶ�ӽڵ�
    int right;  // ���ӽڵ�������-1 ��ʾҶ�ӽڵ�
    int start;  // Ҷ�ӽڵ��е���������ʼ����
    int count;  // Ҷ�ӽڵ��е�����������
    float pad3;
    float pad4;
    float pad5;
    float pad6;
};

class BVHTree {
public:
    std::vector<BVHNode> nodes;
    std::vector<int> triangleIndices; // �洢�����ε�����

    BVHTree(const std::vector<Triangle>& triangles);

private:
    int buildNode(int start, int end, const std::vector<Triangle>& triangles);
    AABB computeAABB(int start, int end, const std::vector<Triangle>& triangles);
};
