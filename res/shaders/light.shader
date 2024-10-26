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
    FragPos = vec3(u_Model * vec4(a_Position, 1.0)); // 修改
    Normal = mat3(transpose(inverse(u_Model))) * a_Normal; // 修改
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

// 新增光照相关uniform
uniform vec3 lightDir;       // 方向光方向
uniform vec3 lightAmbient;   // 环境光强度
uniform vec3 lightDiffuse;   // 漫反射光强度
uniform vec3 lightSpecular;  // 镜面光强度

uniform vec3 viewPos;        // 观察者位置

void main()
{
    // 采样纹理
    vec3 diffuseColor = texture(diffuseMap, TexCoords).rgb * albedo;
    vec3 normalMapColor = texture(normalMap, TexCoords).rgb;
    vec3 normal = normalize(normalMapColor * 2.0 - 1.0); // 法线映射
    vec3 specularColor = texture(specularMap, TexCoords).rgb; // 采样镜面反射颜色

    // 环境光
    vec3 ambient = lightAmbient * diffuseColor;

    // 漫反射光
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, normalize(-lightDir)), 0.0);
    vec3 diffuse = lightDiffuse * diff * diffuseColor;

    // 镜面光
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(lightDir, norm);
    
    // 使用 roughness 调整 specular exponent
    float specularExponent = mix(32.0, 2.0, roughness); // 32 为高光指数，粗糙度越高，指数越低
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), specularExponent);
    
    vec3 specular = lightSpecular * spec * specularColor;

    // 合并所有光照
    vec3 result = ambient + diffuse + specular;
    
    // 考虑金属度和粗糙度（简单处理，可根据PBR需求进一步调整）
    result = mix(diffuseColor, result, metallic);

    FragColor = vec4(result, 1.0);  // 最终颜色
}

