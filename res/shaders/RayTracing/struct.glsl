
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
    int left;   // ���ӽڵ�������-1 ��ʾҶ�ӽڵ�
    int right;  // ���ӽڵ�������-1 ��ʾҶ�ӽڵ�
    int start;  // Ҷ�ӽڵ��е���������ʼ����
    int count;  // Ҷ�ӽڵ��е�����������
    float pad3; // ����Զ���
    float pad4;
    float pad5;
    float pad6;
};

struct OctreeNode {
    AABB bounds;
    int children[8]; // 8���ӽڵ�����, -1��ʾ���ӽڵ�
    int start;  
    int count;
    float pad3;
    float pad4;
};

// ��Դ�ṹ��
struct DirectionalLight
{
    vec3 lightDir;       // ����ⷽ��
    vec3 lightAmbient;   // ������ǿ��
    vec3 lightDiffuse;   // �������ǿ��
    vec3 lightSpecular;  // �����ǿ��
};

struct PointLight
{
    vec3 lightPos;
    vec3 lightAmbient;   // ������ǿ��
    vec3 lightDiffuse;   // �������ǿ��
    vec3 lightSpecular;  // �����ǿ��
    float constant;      // ����˥��
    float linear;        // ����˥��
    float quadratic;     // ���η�˥��
};

struct Ray {
    vec3 origin;
    vec3 dir;
};