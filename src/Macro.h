#pragma once

#include <GL/glew.h>

#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

#define WINDOW_WIDTH 2560
#define WINDOW_HEIGHT 1440

#define SHADOW_WIDTH 1080
#define SHADOW_HEIGHT 1080

#define FAR_PLANE 150
#define NEAR_PLANE 0.1


#define MAX_POINT_LIGHTS 3 // �����Դ����
#define MAX_DIRECTIONAL_LIGHTS 1 //����������
#define MAX_SPOT_LIGHTS 3 // ���۹������
