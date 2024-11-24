#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 FragPos;
uniform mat4 u_Model;

void main()
{
    
    gl_Position = vec4(aPos, 1.0);
    TexCoords = aTexCoords;
    FragPos = vec3(u_Model * vec4(aPos, 1.0));
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;

uniform sampler2D screenTexture;
uniform sampler2D depthTexture;

bool IsVisible(vec3 fragPos);
void main()
{
    FragColor = texture(screenTexture, TexCoords);
}

bool IsVisible(vec3 fragPos){
    float currentDepth = gl_FragCoord.z; // 当前片段的深度值
    float bufferDepth = texture(depthTexture, TexCoords).r; // 从深度纹理中采样的深度值
    return currentDepth + 0.0005 <= bufferDepth;
}


