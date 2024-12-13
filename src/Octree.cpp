#include "Octree.h"

Octree::Octree(const std::vector<Triangle>& triangles, int maxDepth, int leafThreshold) {
    // ��ʼ�� triangleIndices
    triangleIndices.reserve(triangles.size());
    for (int i = 0; i < (int)triangles.size(); ++i) {
        triangleIndices.push_back(i);
    }

    // ���� Octree
    buildNode(0, (int)triangleIndices.size(), triangles, 0, maxDepth, leafThreshold);
}

int Octree::buildNode(int start, int end, const std::vector<Triangle>& triangles, int depth, int maxDepth, int leafThreshold) {
    OctreeNode node;
    node.bounds = computeAABB(start, end, triangles);

    int nodeIndex = (int)nodes.size();
    nodes.push_back(node);

    int count = end - start;
    // �ж��Ƿ�ΪҶ�ӽڵ�����������С�ڵ�����ֵ���ߴﵽ������
    if (count <= leafThreshold || depth >= maxDepth) {
        for (int i = 0; i < 8; i++) {
            nodes[nodeIndex].children[i] = -1;
        }
        nodes[nodeIndex].start = start;
        nodes[nodeIndex].count = count;
        return nodeIndex;
    }

    // ����˸��ӽڵ��AABB
    AABB childAABBs[8];
    subdivideAABB(nodes[nodeIndex].bounds, childAABBs);

    // ���ӽڵ����������
    std::vector<int> childTriangleIndices[8];
    childTriangleIndices->reserve(count);

    for (int i = start; i < end; ++i) {
        int triIdx = triangleIndices[i];
        const Triangle& tri = triangles[triIdx];
        // ��������������
        glm::vec3 centroid = (tri.position[0] + tri.position[1] + tri.position[2]) / 3.0f;

        // �ж����������ĸ��ӽڵ���
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

    // д��triangleIndices�������Ա������������乹���ӽڵ�
    int currentStart = start;
    for (int c = 0; c < 8; ++c) {
        int childCount = (int)childTriangleIndices[c].size();
        // ���������ε��ӽڵ���Ϊ -1
        if (childCount == 0) {
            nodes[nodeIndex].children[c] = -1;
            continue;
        }

        // ���������ο���������
        for (int k = 0; k < childCount; ++k) {
            triangleIndices[currentStart + k] = childTriangleIndices[c][k];
        }

        // �����ӽڵ�
        nodes[nodeIndex].children[c] = buildNode(currentStart, currentStart + childCount, triangles, depth + 1, maxDepth, leafThreshold);
        currentStart += childCount;
    }

    // ��Ҷ�ӽڵ㲻��Ҫstart��count
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

    // �˸��������AABB
    // ��������ֲ��� 
    // index���� (����x:0��1��,y:0��1��,z:0��1ǰ)
    // childIndex = (x_bit << 2) | (y_bit << 1) | z_bit
    // ���� child 0: xyzȫ����0����Ӧmin��center
    // child 1: z=1, ����0, ��Ӧ(min.x, min.y, center.z)��(center.x, center.y, max.z)
    // ... �Դ�����
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