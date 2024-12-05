#shader vertex
#version 450 core
#include "layout.shader"

uniform mat4 SpaceMatrix;

void main()
{
    gl_Position = SpaceMatrix *  a_Position;
}

#shader fragment
#version 450 core

void main()
{
    // 不需要输出颜色，只需要深度值
}
