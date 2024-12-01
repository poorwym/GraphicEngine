#shader vertex
#version 450 core

struct Material {
    float AlbedoMapIndex; //0
    float NormalMapIndex; //1
    float MetallicMapIndex; //2
    float RoughnessMapIndex; //3
    float AOMapIndex; //4
    float EmissionMapIndex; //5
    float AlphaMapIndex; //6
    float padding;
};

struct Triangle {
    vec4 position[3];
    vec4 normal;
    vec4 texCoords[3];
    vec4 tangent;
    vec4 bitangent;
    Material material;
};

layout(std430, binding = 0) buffer TriangleBuffer {
    Triangle triangles[];
};

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

struct Material {
    float AlbedoMapIndex; //0
    float NormalMapIndex; //1
    float MetallicMapIndex; //2
    float RoughnessMapIndex; //3
    float AOMapIndex; //4
    float EmissionMapIndex; //5
    float AlphaMapIndex; //6
    float padding;
};

#define ALBEDO_MAP_INDEX 0
#define NORMAL_MAP_INDEX 1
#define METALLIC_MAP_INDEX 2
#define ROUGHNESS_MAP_INDEX 3
#define AO_MAP_INDEX 4
#define EMISSION_MAP_INDEX 5
#define ALPHA_MAP_INDEX 6

struct Triangle {
    vec4 position[3];
    vec4 normal;
    vec4 texCoords[3];
    vec4 tangent;
    vec4 bitangent;
    Material material;
};

layout(std430, binding = 0) buffer TriangleBuffer {
    Triangle triangles[];
};
uniform int numTriangles;

uniform sampler2DArray textures;
uniform float farPlane;

// 光源结构体
struct DirectionalLight
{
    vec3 lightDir;       // 方向光方向
    vec3 lightAmbient;   // 环境光强度
    vec3 lightDiffuse;   // 漫反射光强度
    vec3 lightSpecular;  // 镜面光强度
};
uniform DirectionalLight directionalLight;

#define MAX_POINT_LIGHTS 3
struct PointLight
{
    vec3 lightPos;
    vec3 lightAmbient;   // 环境光强度
    vec3 lightDiffuse;   // 漫反射光强度
    vec3 lightSpecular;  // 镜面光强度
    float constant;      // 常数衰减
    float linear;        // 线性衰减
    float quadratic;     // 二次方衰减
};
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int numPointLights;

uniform vec3 viewPos;        // 观察者位置
uniform vec3 cameraPosition; // 相机位置
uniform float nearPlane;    // 近裁剪面
uniform int screenWidth;    // 屏幕宽度
uniform int screenHeight;   // 屏幕高度
uniform float fov;          // 视角
uniform float aspectRatio;  // 屏幕宽高比
uniform mat4 viewMatrix;    // 视图矩阵

uniform float seed;

#define MAX_DEPTH 2  // 最大递归深度
const float PI = 3.14159265359;

struct Ray {
    vec3 origin;
    vec3 dir;
};

float rand(vec2 seed){
    return fract(sin(dot(seed, vec2(12.9898,78.233))) * 43758.5453);
}

vec3 GetTextureColor(int index, vec2 texCoords, int slot){
    if(slot== 0)//albedo
    {
        if(triangles[index].material.AlbedoMapIndex != -1 ) return texture(textures, vec3(texCoords, triangles[index].material.AlbedoMapIndex)).rgb;
    }
    else if(slot== 1)// normal
    {
        if(triangles[index].material.NormalMapIndex != -1 ) return texture(textures, vec3(texCoords, triangles[index].material.NormalMapIndex)).rgb;
        return triangles[index].normal.xyz;
    }
    else if(slot== 5)// emission
    {
        if(triangles[index].material.EmissionMapIndex != -1 ) return texture(textures, vec3(texCoords, triangles[index].material.EmissionMapIndex)).rgb;
        return vec3(0.0);
    }
    return vec3(0.0);
}
float GetTextureValue(int index, vec2 texCoords, int slot){
    if( slot == 2)// metallic
    {
        if(triangles[index].material.MetallicMapIndex != -1 ) return texture(textures, vec3(texCoords, triangles[index].material.MetallicMapIndex)).r;
        return 0.0;
    }
    else if( slot == 3)// roughness
    {
        if(triangles[index].material.RoughnessMapIndex != -1 ) return texture(textures, vec3(texCoords, triangles[index].material.RoughnessMapIndex)).r;
        return 1.0;
    }
    else if( slot == 4)// ao
    {
        if(triangles[index].material.AOMapIndex != -1 ) return texture(textures, vec3(texCoords, triangles[index].material.AOMapIndex)).r;
        return 1.0;
    }
    else if( slot == 6)// alpha
    {
        if(triangles[index].material.AlphaMapIndex != -1 ) return texture(textures, vec3(texCoords, triangles[index].material.AlphaMapIndex)).r;
        return 1.0;
    }
    return 0.0;
}
vec3 TraceRay(Ray ray);
vec3 CalculateAmbientColor(vec3 albedo, vec3 ambientLight, float AO);// 环境光
vec3 CalculateDiffuseColor(vec3 albedo, vec3 lightDiffuse, vec3 lightDir, vec3 normal);// 漫反射光
vec3 CalculateSpecularColor(vec3 albedo, vec3 lightSpecular, vec3 lightDir, vec3 normal, vec3 viewDir, float roughness, float metallic);// 镜面光
float CalculateDirectionShadow(vec3 hitPoint, vec3 lightDir, vec3 normal);// 计算方向光阴影
float CalculatePointShadow(vec3 hitPoint, vec3 lightDir, float distance, vec3 normal); //计算点光源阴影

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
    vec3 finalColor = TraceRay(ray);
    
    FragColor = vec4(finalColor, 1.0);
}

bool RayIntersectsTriangle(Ray ray, Triangle tri, out float t, out vec3 hitNormal, out vec2 hitTexCoord, int index) // 使用 Möller-Trumbore 算法
{
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

        // 构建 TBN 矩阵
        mat3 TBN = mat3(tri.tangent.xyz, tri.bitangent.xyz, tri.normal.xyz);

        // 获取法线贴图中的法线
        vec3 tangentNormal = GetTextureColor(index, hitTexCoord, NORMAL_MAP_INDEX);
        // 将法线从 [0,1] 映射到 [-1,1]
        tangentNormal = tangentNormal * 2.0 - 1.0;
        // 计算世界空间的法线
        hitNormal = normalize(TBN * tangentNormal);
        return true;
    }
    else
        return false;
}

bool SceneIntersection(Ray ray, out int hitIndex, out float tMin, out vec3 hitNormal, out vec2 hitTexCoord)
{
    tMin = 10000;
    bool hit = false;
    for (int i = 0; i < numTriangles; ++i)
    {
        float t;
        vec3 normal;
        vec2 texCoords;
        if (RayIntersectsTriangle(ray, triangles[i], t, normal, texCoords, i))
        {
            if (t < tMin)
            {
                tMin = t;
                hitIndex = i;
                hitNormal = normal;
                hitTexCoord = texCoords;
                hit = true;
            }
        }
    }
    return hit;
}
vec3 TraceRay(Ray ray)
{
    vec3 radiance = vec3(0.0);
    vec3 throughput = vec3(1.0);
    for (int depth = 0; depth < MAX_DEPTH; ++depth)
    {
        int hitIndex;
        float t;
        vec3 hitNormal;
        vec2 hitTexCoord;
        if (SceneIntersection(ray, hitIndex, t, hitNormal, hitTexCoord))
        {
            vec3 albedo = GetTextureColor(hitIndex, hitTexCoord, ALBEDO_MAP_INDEX);
            vec3 normal = hitNormal;
            float metallic = GetTextureValue(hitIndex, hitTexCoord, METALLIC_MAP_INDEX);
            float roughness = GetTextureValue(hitIndex, hitTexCoord, ROUGHNESS_MAP_INDEX);
            float AO = GetTextureValue(hitIndex, hitTexCoord, AO_MAP_INDEX);
            vec3 emission = GetTextureColor(hitIndex, hitTexCoord, EMISSION_MAP_INDEX);
            float alpha = GetTextureValue(hitIndex, hitTexCoord, ALPHA_MAP_INDEX);
            // 计算交点
            vec3 hitPoint = ray.origin + ray.dir * t + normal * 0.0001;
            // 计算方向光
            vec3 viewDir = normalize(ray.dir);// 从光线起点到交点的向量
            vec3 ambientColor = CalculateAmbientColor(albedo, directionalLight.lightAmbient, AO);
            vec3 diffuseColor = CalculateDiffuseColor(albedo, directionalLight.lightDiffuse, directionalLight.lightDir, normal);
            vec3 specularColor = CalculateSpecularColor(albedo, directionalLight.lightSpecular, directionalLight.lightDir, normal, viewDir, roughness, metallic);
            float dirLightShadow = CalculateDirectionShadow(hitPoint, directionalLight.lightDir, normal);
            radiance += throughput * (emission + ambientColor + (1 - dirLightShadow) * (diffuseColor + specularColor));

            // 计算点光源
            for(int i = 0; i < numPointLights; ++i){
                vec3 lightDir = normalize(pointLights[i].lightPos - hitPoint);
                vec3 pointDiffuse = CalculateDiffuseColor(albedo, pointLights[i].lightDiffuse, lightDir, normal);
                vec3 pointSpecular = CalculateSpecularColor(albedo, pointLights[i].lightSpecular, lightDir, normal, viewDir, roughness, metallic);
                float distance = distance(hitPoint, pointLights[i].lightPos);
                float attenuation = 1.0 / (pointLights[i].constant + pointLights[i].linear * distance + pointLights[i].quadratic * (distance * distance));
                float pointShadow = CalculatePointShadow(hitPoint, pointLights[i].lightPos, distance, normal);
                pointDiffuse = pointDiffuse * attenuation;
                pointSpecular = pointSpecular * attenuation;
                radiance += (1 - pointShadow) * (pointDiffuse + pointSpecular);
            }
            
            throughput *= 0.1;

            ray.origin += ray.dir * t + normal * 0.0001;
            ray.dir = reflect(ray.dir, normal);
        }
        else
        {
            radiance += throughput * vec3(1.0);
            break;
        }
    }
    return radiance;
}



vec3 CalculateAmbientColor(vec3 albedo, vec3 lightAmbient, float AO){
    //ambient color
    vec3 ambientColor = lightAmbient * albedo * AO;
    return ambientColor;
}
vec3 CalculateDiffuseColor(vec3 albedo, vec3 lightDiffuse, vec3 lightDir, vec3 normal){
    vec3 N = normalize(normal);
    vec3 L = normalize(lightDir);
    vec3 diffuseColor = lightDiffuse * albedo * max(dot(lightDir, normal), 0);
    return diffuseColor;
}

// 几何遮蔽函数：Schlick-GGX
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

// 几何遮蔽函数：Smith
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// Fresnel 方程：Schlick近似
vec3 fresnelSchlick(float cosTheta, float metallic, vec3 albedo)
{
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
float GGXDistribution(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
vec3 CalculateSpecularColor(vec3 albedo, vec3 lightSpecular, vec3 lightDir, vec3 normal, vec3 viewDir, float roughness, float metallic){
    vec3 V = normalize(viewDir);
    vec3 L = normalize(lightDir);
    vec3 H = normalize(V + L);
    vec3 N = normalize(normal);
    vec3 F = fresnelSchlick(abs(dot(H, V)), metallic, albedo);
    float G = GeometrySmith(N, V, L, roughness);
    float D = GGXDistribution(N, H, roughness);
    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 0.0);
    vec3 specularColor = lightSpecular * F * D * G / max(4.0 * NdotL * NdotV, 0.001);
    return specularColor;
}

#define PCF_SAMPLES 4  // PCF 采样次数（例如 2x2 网格）

const float shadowEpsilon = 0.0001; // 防止自我遮挡的小偏移量

// 预定义的采样偏移（2x2 网格）
vec2 pcfOffsets[PCF_SAMPLES] = vec2[](vec2(-0.25, -0.25),
                                     vec2(0.25, -0.25),
                                     vec2(-0.25, 0.25),
                                     vec2(0.25, 0.25));

float CalculateDirectionShadow(vec3 hitPoint, vec3 lightDir, vec3 normal){
    float shadow = 0.0;

    // 定义采样半径，可以根据需要调整
    float radius = 0.05;

    // 生成正交基（tangent 和 bitangent）
    vec3 up = abs(lightDir.y) < 0.99 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, lightDir));
    vec3 bitangent = cross(lightDir, tangent);

    for(int i = 0; i < PCF_SAMPLES; ++i){
        // 生成随机偏移
        float randX = rand(gl_FragCoord.xy + float(i));
        float randY = rand(gl_FragCoord.xy + float(i) + 1.0);
        vec2 jitter = vec2(randX, randY) * 2.0 - vec2(1.0); // 范围 [-1, 1]
        vec2 offset = pcfOffsets[i] * radius + jitter * (radius * 0.5); // 结合预定义偏移和随机抖动

        // 生成偏移后的光线方向
        vec3 perturbedLightDir = normalize(lightDir + tangent * offset.x + bitangent * offset.y);

        // 偏移光线起点，避免自我遮挡
        vec3 origin = hitPoint + normal * shadowEpsilon;

        // 构建阴影光线
        Ray shadowRay;
        shadowRay.origin = origin;
        shadowRay.dir = perturbedLightDir;

        float t1, t2;
        vec3 t3;
        vec2 t4;

        // 进行光线与场景的相交测试
        shadow += SceneIntersection(shadowRay, t1, t2, t3, t4) ? 1.0 : 0.0;
    }

    // 计算平均阴影因子
    shadow /= float(PCF_SAMPLES);

    return shadow; // 返回 0.0（无遮挡）到 1.0（完全遮挡）的阴影因子
}

float CalculatePointShadow(vec3 hitPoint, vec3 lightDir, float distance, vec3 normal){
    float shadow = 0.0;

    // 定义采样半径，可以根据需要调整
    float radius = 0.05;

    // 计算原始光源位置
    vec3 originalLightPos = hitPoint + lightDir * distance;

    // 生成正交基（tangent 和 bitangent）
    vec3 up = abs(lightDir.y) < 0.99 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, lightDir));
    vec3 bitangent = cross(lightDir, tangent);

    for(int i = 0; i < PCF_SAMPLES; ++i){
        // 生成随机偏移
        float randX = rand(gl_FragCoord.xy + float(i));
        float randY = rand(gl_FragCoord.xy + float(i) + 1.0);
        vec2 jitter = vec2(randX, randY) * 2.0 - vec2(1.0); // 范围 [-1, 1]
        vec2 offset = pcfOffsets[i] * radius + jitter * (radius * 0.5); // 结合预定义偏移和随机抖动

        // 生成偏移后的光源位置
        vec3 perturbedLightPos = originalLightPos + tangent * offset.x + bitangent * offset.y;

        // 计算新的光线方向和新的距离
        vec3 newLightDir = normalize(perturbedLightPos - hitPoint);
        float newDistance = length(perturbedLightPos - hitPoint);

        // 偏移光线起点，避免自我遮挡
        vec3 origin = hitPoint + newLightDir * shadowEpsilon;

        // 构建阴影光线
        Ray shadowRay;
        shadowRay.origin = origin;
        shadowRay.dir = newLightDir;

        float t1, t_distance;
        vec3 t3;
        vec2 t4;

        // 进行光线与场景的相交测试
        if(SceneIntersection(shadowRay, t1, t_distance, t3, t4)){
            if(t_distance < newDistance){
                shadow += 1.0; // 遮挡
            }
        }
    }

    // 计算平均阴影因子
    shadow /= float(PCF_SAMPLES);

    return shadow; // 返回 0.0（无遮挡）到 1.0（完全遮挡）的阴影因子
}