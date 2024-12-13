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
uniform float maxBlur;      // 最大模糊半径（以像素为单位）
uniform vec2 texelSize;     // 纹素大小（1.0 / 纹理分辨率）

vec3 ApplyDepthOfField(vec2 texCoords, float blurRadius);

void main()
{
    // 获取当前片段的深度
    float fragDepth = texture(depthTexture, TexCoords).r;

    // 计算模糊量
    float depthDiff = abs(fragDepth - focusDepth);
    float blurAmount = smoothstep(focusRange, 1.0, depthDiff);
    float blurRadius = blurAmount * maxBlur;

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

    // 采样偏移数组（泊松盘采样）
    const int sampleCount = 16; // 采样点数
    vec2 poissonDisk[sampleCount] = vec2[](
        vec2(-0.94201624, -0.39906216),
        vec2(0.94558609, -0.76890725),
        vec2(-0.09418410, -0.92938870),
        vec2(0.34495938, 0.29387760),
        vec2(-0.91588581, 0.45771432),
        vec2(-0.81544232, -0.87912464),
        vec2(-0.38277543, 0.27676845),
        vec2(0.97484398, 0.75648379),
        vec2(0.44323325, -0.97511554),
        vec2(0.53742981, -0.47373420),
        vec2(-0.26496911, -0.41893023),
        vec2(0.79197514, 0.19090188),
        vec2(-0.24188840, 0.99706507),
        vec2(-0.81409955, 0.91437590),
        vec2(0.19984126, 0.78641367),
        vec2(0.14383161, -0.14100790)
    );

    for (int i = 0; i < sampleCount; ++i) {
        vec2 offset = poissonDisk[i] * blurRadius * texelSize;
        vec3 sampleColor = texture(screenTexture, texCoords + offset).rgb;

        // 高斯权重计算
        float weight = exp(-dot(poissonDisk[i], poissonDisk[i]) / 2.0);
        result += sampleColor * weight;
        totalWeight += weight;
    }

    // 归一化
    return result / totalWeight;
}