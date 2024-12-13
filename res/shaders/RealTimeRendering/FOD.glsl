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

// �������
uniform float focusDepth;   // �������
uniform float focusRange;   // ���㷶Χ����������
uniform float maxBlur;      // ���ģ���뾶

vec3 ApplyDepthOfField(vec2 texCoords, float blurRadius);

void main()
{
    // ��ȡ��ǰƬ�ε����
    float fragDepth = texture(depthTexture, TexCoords).r;

    // ����ģ���뾶
    float depthDiff = abs(fragDepth - focusDepth);
    depthDiff = min(depthDiff, 1.0);
    depthDiff = pow(depthDiff, 2);
    float t = max(0, depthDiff - focusRange);
    float blurRadius = t * maxBlur;

    // Ӧ�þ���Ч��
    vec3 color = ApplyDepthOfField(TexCoords, blurRadius);
    FragColor = vec4(color, 1.0);
}

vec3 ApplyDepthOfField(vec2 texCoords, float blurRadius)
{
    if (blurRadius < 0.01) {
        // ���ģ���뾶��С��ֱ�ӷ���ԭʼ����
        return texture(screenTexture, texCoords).rgb;
    }

    // ��ʼģ������
    vec3 result = vec3(0.0);
    float totalWeight = 0.0;

    // ����ƫ������
    const int sampleCount = 8; // ��������
    vec2 offsets[sampleCount] = vec2[](
        vec2(-1.0, -1.0), vec2(-1.0, 1.0), 
        vec2(1.0, -1.0), vec2(1.0, 1.0),
        vec2(-1.0, 0.0), vec2(1.0, 0.0),
        vec2(0.0, -1.0), vec2(0.0, 1.0)
    );

    // �Բ�������м�Ȩģ��
    for (int i = 0; i < sampleCount; ++i) {
        vec2 offset = offsets[i] * blurRadius / float(sampleCount);
        vec3 sampleColor = texture(screenTexture, texCoords + offset).rgb;

        // �򵥵�Ȩ�ؼ��㣨�ɵ�����
        float weight = 1.0 - length(offset) / blurRadius;
        result += sampleColor * weight;
        totalWeight += weight;
    }

    // ��һ��
    return result / totalWeight;
}
