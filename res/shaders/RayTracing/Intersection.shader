// AABB 与 Ray 的相交测试函数
bool AABBIntersect(Ray ray, AABB aabb, out float tMin, out float tMax) {
    // 初始化 tMin 和 tMax
    tMin = 0.0;
    tMax = 10000.0; // 根据场景需求调整最大值

    // 对每个轴进行相交测试
    for(int i = 0; i < 3; i++) {
        float invD = 1.0 / ray.dir[i];
        float t0 = (aabb.min[i] - ray.origin[i]) * invD;
        float t1 = (aabb.max[i] - ray.origin[i]) * invD;

        // 确保 t0 是较小的值
        if(invD < 0.0){
            float temp = t0;
            t0 = t1;
            t1 = temp;
        }

        // 更新 tMin 和 tMax
        tMin = max(tMin, t0);
        tMax = min(tMax, t1);

        // 如果没有相交
        if(tMax < tMin){
            return false;
        }
    }

    return true;
}

bool BVHIntersection(Ray ray, out int hitIndex, out float tMin, out vec3 hitNormal, out vec2 hitTexCoord) {
    tMin = 10000.0;
    hitIndex = -1;
    bool hit = false;

    // 使用栈来遍历 BVH 树
    int stack[64];
    int stackPtr = 0;
    stack[stackPtr++] = 0; // 假设根节点的索引为 0

    while (stackPtr > 0) {
        int nodeIdx = stack[--stackPtr];
        BVHNode node = nodes[nodeIdx];

        // 相交测试
        float t0, t1;
        if (!AABBIntersect(ray, node.bounds, t0, t1)) {
            continue;
        }

        if (node.left == -1 && node.right == -1) { // 叶子节点
            for (int i = node.start; i < node.start + node.count; ++i) {
                int triIdx = triangleIndices[i];
                Triangle tri = triangles[triIdx];
                float t;
                vec3 normal;
                vec2 texCoord;
                if (RayIntersectsTriangle(ray, tri, t, normal, texCoord, triIdx)) {
                    if (t < tMin && t > 0.0001) {
                        tMin = t;
                        hitIndex = triIdx;
                        hitNormal = normal;
                        hitTexCoord = texCoord;
                        hit = true;
                    }
                }
            }
        } else {
            // 先遍历左子节点，再右子节点
            if (node.left != -1) {
                stack[stackPtr++] = node.left;
            }
            if (node.right != -1) {
                stack[stackPtr++] = node.right;
            }
        }
    }

    return hit;
}
bool RayIntersectsTriangle(Ray ray, Triangle tri, out float t, out vec3 hitNormal, out vec2 hitTexCoord, int index)
{
    // Möller-Trumbore 算法实现
    vec3 edge1 = (tri.position[1] - tri.position[0]).xyz;
    vec3 edge2 = (tri.position[2] - tri.position[0]).xyz;
    vec3 h = cross(ray.dir, edge2);
    float a = dot(edge1, h);
    if (abs(a) < 0.0001)
        return false;
    float f = 1.0 / a;
    vec3 s = ray.origin - tri.position[0].xyz;
    float u = f * dot(s, h);
    if (u < 0.0 || u > 1.0)
        return false;
    vec3 q = cross(s, edge1);
    float v = f * dot(ray.dir, q);
    if (v < 0.0 || u + v > 1.0)
        return false;
    t = f * dot(edge2, q);
    if (t > 0.0001)
    {
        // 计算插值的纹理坐标
        hitTexCoord = (1.0 - u - v) * tri.texCoords[0].xy + u * tri.texCoords[1].xy + v * tri.texCoords[2].xy;

        // **在此处获取并检查 alpha 值，无论是否有法线贴图**
        float alpha = GetAlpha(index, hitTexCoord);
        if(alpha < 0.5){
            return false;
        }

        // 插值法线、切线和双切线
        vec3 n0 = tri.normal[0].xyz;
        vec3 n1 = tri.normal[1].xyz;
        vec3 n2 = tri.normal[2].xyz;
        vec3 interpolatedNormal = normalize((1.0 - u - v) * n0 + u * n1 + v * n2);

        vec3 t0 = tri.tangent[0].xyz;
        vec3 t1 = tri.tangent[1].xyz;
        vec3 t2 = tri.tangent[2].xyz;
        vec3 interpolatedTangent = normalize((1.0 - u - v) * t0 + u * t1 + v * t2);

        vec3 b0 = tri.bitangent[0].xyz;
        vec3 b1 = tri.bitangent[1].xyz;
        vec3 b2 = tri.bitangent[2].xyz;
        vec3 interpolatedBitangent = normalize((1.0 - u - v) * b0 + u * b1 + v * b2);

        // 构建 TBN 矩阵
        mat3 TBN = mat3(interpolatedTangent, interpolatedBitangent, interpolatedNormal);

        vec3 combinedNormalTS = interpolatedNormal;

        // 从法线贴图中获取法线并结合高度贴图的效果
        if(triangles[index].material.NormalMapIndex != -1){
            vec3 normalMapNormalTS = GetTextureColor(index, hitTexCoord, NORMAL_MAP_INDEX);
            normalMapNormalTS = normalMapNormalTS * 2.0 - 1.0; // 将法线从 [0,1] 映射到 [-1,1]

            // 将法线贴图的法线与高度贴图的法线结合
            combinedNormalTS = normalize(TBN * normalMapNormalTS);
        }

        
        hitNormal = combinedNormalTS;
        return true;
    }
    else
        return false;
}