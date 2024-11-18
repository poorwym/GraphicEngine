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

#define FAR_PLANE 20
#define NEAR_PLANE 0.1