#shader vertex
#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;
void main()
{
    gl_Position = vec4(aPos, 1.0);
    TexCoords = aTexCoords;
}

#shader fragment
#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D depthTexture;

// 景深参数
uniform float focusDepth;   // 焦点深度
uniform float focusRange;   // 焦点范围（过渡区域）
uniform float maxBlur;      // 最大模糊半径

vec3 ApplyDepthOfField(vec2 texCoords, float blurRadius);

void main()
{
    // 获取当前片段的深度
    float fragDepth = texture(depthTexture, TexCoords).r;

    // 计算模糊半径
    float depthDiff = abs(fragDepth - focusDepth);
    depthDiff = min(depthDiff, 1.0);
    depthDiff = pow(depthDiff, 2);
    float t = max(0, depthDiff - focusRange);
    float blurRadius = t * maxBlur;

    // 应用景深效果
    vec3 color = ApplyDepthOfField(TexCoords, blurRadius);
    FragColor = vec4(color, 1.0);
}

vec3 ApplyDepthOfField(vec2 texCoords, float blurRadius)
{
    if (blurRadius < 0.01) {
        // 如果模糊半径很小，直接返回原始纹理
        return texture(screenTexture, texCoords).rgb;
    }

    // 开始模糊处理
    vec3 result = vec3(0.0);
    float totalWeight = 0.0;

    // 采样偏移数组
    const int sampleCount = 8; // 采样点数
    vec2 offsets[sampleCount] = vec2[](
        vec2(-1.0, -1.0), vec2(-1.0, 1.0), 
        vec2(1.0, -1.0), vec2(1.0, 1.0),
        vec2(-1.0, 0.0), vec2(1.0, 0.0),
        vec2(0.0, -1.0), vec2(0.0, 1.0)
    );

    // 对采样点进行加权模糊
    for (int i = 0; i < sampleCount; ++i) {
        vec2 offset = offsets[i] * blurRadius / float(sampleCount);
        vec3 sampleColor = texture(screenTexture, texCoords + offset).rgb;

        // 简单的权重计算（可调整）
        float weight = 1.0 - length(offset) / blurRadius;
        result += sampleColor * weight;
        totalWeight += weight;
    }

    // 归一化
    return result / totalWeight;
}
