#shader vertex
#version 330 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoords;
layout (location = 3) in float a_Slot0;
layout (location = 4) in float a_Slot1;
layout (location = 5) in float a_Slot2;
layout (location = 6) in float a_Slot3;
layout (location = 7) in float a_Slot4;
layout (location = 8) in float a_Slot5;
layout (location = 9) in float a_Slot6;

uniform mat4 SpaceMatrix;

void main()
{
    gl_Position = SpaceMatrix *  vec4(a_Position, 1.0);
}

#shader fragment
#version 330 core

void main()
{
    // 不需要输出颜色，只需要深度值
}
