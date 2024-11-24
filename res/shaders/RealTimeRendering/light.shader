#shader vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;

uniform mat4 u_MVP;
uniform mat4 u_Model;    

out vec3 FragPos; // 传递给片段着色器世界位置
out vec3 Normal;  // 传递给片段着色器法线向量
out vec2 TexCoords; // 传递给片段着色器纹理坐标

void main()
{
    gl_Position = u_MVP * vec4(a_Position, 1.0);
    FragPos = vec3(u_Model * vec4(a_Position, 1.0)); // 世界位置
    Normal = mat3(transpose(inverse(u_Model))) * a_Normal; // 世界法线
    TexCoords = a_TexCoords;
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords; // 片段着色器接受的纹理坐标
in vec3 FragPos;   // 片段着色器接受的世界位置
in vec3 Normal;    // 片段着色器接受的法线向量

uniform sampler2D diffuseMap;   // 漫反射纹理
uniform sampler2D normalMap;    // 法线贴图
uniform sampler2D specularMap;  // 镜面反射贴图

uniform vec3 albedo;
uniform float metallic;
uniform float roughness;

// 方向光结构体
struct DirectionalLight
{
    vec3 lightDir;       // 方向光方向
    vec3 lightAmbient;   // 环境光强度
    vec3 lightDiffuse;   // 漫反射光强度
    vec3 lightSpecular;  // 镜面光强度
};
uniform DirectionalLight directionalLight;

#define MAX_POINT_LIGHTS 4
struct PointLight
{
    vec3 lightPos;
    vec3 lightAmbient;   // 环境光强度
    vec3 lightDiffuse;   // 漫反射光强度
    vec3 lightSpecular;  // 镜面光强度
    float constant;   //常数衰减
    float linear;     //线性衰减
    float quadratic;  //二次方衰减
};

uniform PointLight pointLights[MAX_POINT_LIGHTS];

uniform vec3 viewPos;        // 观察者位置
uniform int numPointLights;

void main()
{
    // 采样纹理
    vec3 diffuseColor = texture(diffuseMap, TexCoords).rgb * albedo;
    vec3 normalMapColor = texture(normalMap, TexCoords).rgb;
    vec3 normal = normalize(normalMapColor * 2.0 - 1.0); // 法线映射
    vec3 specularColor = texture(specularMap, TexCoords).rgb; // 采样镜面反射颜色

    // 环境光
    vec3 ambient = directionalLight.lightAmbient * diffuseColor;

    // 漫反射光
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, normalize(-directionalLight.lightDir)), 0.0);
    vec3 diffuse = directionalLight.lightDiffuse * diff * diffuseColor;

    // 镜面光
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(directionalLight.lightDir, norm);
    
    // 使用 roughness 调整 specular exponent
    float specularExponent = mix(16.0, 2.0, roughness); // 16 为高光指数，粗糙度越高，指数越低
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), specularExponent);
    
    vec3 specular = directionalLight.lightSpecular * spec * specularColor;

    //所有点光源的贡献
    for(int i = 0; i < numPointLights; i++)
    {
        // 光源方向和距离
        vec3 lightDir = normalize(pointLights[i].lightPos - FragPos);
        float distance = length(pointLights[i].lightPos - FragPos);

        // 衰减
        float attenuation = 1.0 / (pointLights[i].constant + pointLights[i].linear * distance + 
                                   pointLights[i].quadratic * (distance * distance));

        // 环境光
        vec3 ambientComponent = pointLights[i].lightAmbient * diffuseColor;

        // 漫反射光
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuseComponent = pointLights[i].lightDiffuse * diff * diffuseColor;

        // 镜面光
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), specularExponent);
        vec3 specularComponent = pointLights[i].lightSpecular * spec * specularColor;

        // 应用衰减
        ambientComponent *= attenuation;
        diffuseComponent *= attenuation;
        specularComponent *= attenuation;

        // 累加点光源的贡献
        ambient += ambientComponent;
        diffuse += diffuseComponent;
        specular += specularComponent;
    }
    
    // 合并所有光照
    vec3 result = ambient + diffuse + specular;
    
    // 考虑金属度和粗糙度（简单处理，可根据PBR需求进一步调整）
    result = mix(diffuseColor, result, metallic);

    FragColor = vec4(result, 1.0);  // 最终颜色
}

