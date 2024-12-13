
struct Material {
    float AlbedoMapIndex; //0
    float NormalMapIndex; //1
    float MetallicMapIndex; //2
    float RoughnessMapIndex; //3
    float AOMapIndex; //4
    float EmissionMapIndex; //5
    float AlphaMapIndex; //6
    float HeightMapIndex; //7
    //Ka
    vec4 Ambient;
    //Kd
    vec4 Diffuse;
    //Ks
    vec4 Specular;
    //Ke
    vec4 Emission;
    //d
    float Dissolve;
    //illum
    float Illum;
    //Ni
    float OpticalDensity;
    //Ns
    float SpecularExponent;
    float BumpMutiplier;
    float padding2;
    float padding3;
    float padding4;
};

struct Triangle {
    vec4 position[3];
    vec4 normal[3];
    vec4 texCoords[3];
    vec4 tangent[3];
    vec4 bitangent[3];
    int materialIndex;
    float padding1;
    float padding2;
    float padding3;
};

struct AABB {
    vec3 min;
    float pad1;
    vec3 max;
    float pad2;
};

struct BVHNode {
    AABB bounds;
    int left;   // 左子节点索引，-1 表示叶子节点
    int right;  // 右子节点索引，-1 表示叶子节点
    int start;  // 叶子节点中的三角形起始索引
    int count;  // 叶子节点中的三角形数量
    float pad3; // 填充以对齐
    float pad4;
    float pad5;
    float pad6;
};

struct OctreeNode {
    AABB bounds;
    int children[8]; // 8个子节点索引, -1表示无子节点
    int start;  
    int count;
    float pad3;
    float pad4;
};

// 光源结构体
struct DirectionalLight
{
    vec3 lightDir;       // 方向光方向
    vec3 lightAmbient;   // 环境光强度
    vec3 lightDiffuse;   // 漫反射光强度
    vec3 lightSpecular;  // 镜面光强度
};

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

struct Ray {
    vec3 origin;
    vec3 dir;
};