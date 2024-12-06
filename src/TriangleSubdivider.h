
#include <vector>
#include "Mesh.h"

#define MAX_TRIANGLE_EDGE_LENGTH 1.0

class TriangleSubdivider {
public:
    // ϸ�ֵ��������εķ���
    void SubdivideTriangle(
        const Triangle& inputTriangle,
        int subdivisionLevel,
        std::vector<Triangle>& outputTriangles
    );

    Triangle CreateTriangle(Vertex& v1, Vertex& v2, Vertex& v3);
    void AdjustHeight(Triangle& triangle, CPUTexture* heightMap, float bumpMutiplier);

private:
    // �ݹ�ϸ�ֺ���
    void Subdivide(
        const Triangle& triangle,
        int level,
        std::vector<Triangle>& triangles
    );

    // ��������������е�
    Vertex ComputeMidpoint(const Vertex& v1, const Vertex& v2);
};
