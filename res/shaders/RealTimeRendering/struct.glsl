// 方向光源结构体
struct DirectionalLight
{
    vec3 lightDir;       // 方向光方向
    vec3 lightAmbient;   // 环境光强度
    vec3 lightDiffuse;   // 漫反射光强度
    vec3 lightSpecular;  // 镜面光强度
};

// 点光源结构体
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
    float Roughness;
    float Metallic;
};