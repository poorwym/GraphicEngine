#shader vertex
#version 450 core
layout(location = 0) in vec2 aPos;

out VS_OUT {
    vec2 TexCoords;
} vs_out;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    vs_out.TexCoords = (aPos + 1.0) / 2.0; // 将 NDC 坐标转换为纹理坐标 [0, 1]
}

#shader fragment
#version 450 core
out vec4 FragColor;
// 这个顺序不能动
#include "Macro.shader"
#include "struct.shader"
#include "uniform.shader"
#include "Head.shader"

layout(std430, binding = 0) buffer TriangleBuffer {
    Triangle triangles[];
};

layout(std430, binding = 1) buffer BVHTreeBuffer {
    BVHNode nodes[];
};

layout(std430, binding = 2) buffer TriangleIndicesBuffer {
    int triangleIndices[];
};

float rand(vec2 seed){
    return fract(sin(dot(seed, vec2(12.9898,78.233))) * 43758.5453);
}

#include "Intersection.shader"
#include "PBR.shader"
#include "Shadow.shader"
#include "TraceRay.shader"
#include "material.shader"
#include "BRDF.shader"

in VS_OUT {
    vec2 TexCoords;
} vs_out;

void main(){
    // 获取屏幕空间的纹理坐标
    vec2 uv = vec2(gl_FragCoord.x + rand(gl_FragCoord.xy + seed), gl_FragCoord.y + rand(gl_FragCoord.xy + seed));
    uv.x = uv.x / screenWidth;
    uv.y = uv.y / screenHeight;
    
    // 将纹理坐标转换为 NDC 坐标范围 [-1, 1]
    float x_ndc = uv.x * 2.0 - 1.0;
    float y_ndc = uv.y * 2.0 - 1.0;
    
    // 计算视平面的高度和宽度
    float tanFov = tan(radians(fov) / 2.0);
    float viewHeight = 2.0 * tanFov * nearPlane;
    float viewWidth = viewHeight * aspectRatio;
    
    // 将 NDC 坐标映射到视空间坐标
    float x_view = x_ndc * (viewWidth / 2.0);
    float y_view = y_ndc * (viewHeight / 2.0);
    float z_view = -nearPlane; // 视平面位于 -Z 方向
    
    vec4 pixelPosViewSpace = vec4(x_view, y_view, z_view, 1.0);
    
    // 将视空间坐标转换为世界空间坐标
    // 使用视图矩阵的逆矩阵
    mat4 inverseView = inverse(viewMatrix);
    vec4 pixelPosWorldSpace = inverseView * pixelPosViewSpace;
    // 获取世界空间的 pixelPos
    vec3 pixelPos = vec3(pixelPosWorldSpace) / pixelPosWorldSpace.w;
    
    // 计算光线方向并归一化
    vec3 rayDir = normalize(pixelPos - cameraPosition);
    
    // 初始化光线
    Ray ray;
    ray.origin = cameraPosition;
    ray.dir = rayDir;
    
    // 光线追踪
    vec4 finalColor = TraceRay(ray, vec3(1.0));
    FragColor = finalColor;

    //gl_FragDepth = texture(depthMap, uv).r;
}





