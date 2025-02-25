#shader vertex
#version 450 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * model * vec4(aPos, 1.0);
    gl_Position = pos.xyww; // 保持深度为1.0
}

#shader fragment
#version 450 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, TexCoords);
}