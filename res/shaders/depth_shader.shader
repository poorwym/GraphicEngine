#shader vertex
#version 330 core
layout (location = 0) in vec3 a_Position;

uniform mat4 lightSpaceMatrix;
uniform mat4 u_Model;

void main()
{
    gl_Position = lightSpaceMatrix * u_Model * vec4(a_Position, 1.0);
}

#shader fragment
#version 330 core

void main()
{
    // 不需要输出颜色，只需要深度值
}
