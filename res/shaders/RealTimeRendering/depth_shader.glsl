#shader vertex
#version 450 core
#include "layout.glsl"

uniform mat4 SpaceMatrix;

void main()
{
    gl_Position = SpaceMatrix *  a_Position;
}

#shader fragment
#version 450 core

void main()
{
    // ����Ҫ�����ɫ��ֻ��Ҫ���ֵ
}
