#pragma once
#include "glm/glm.hpp"

class InputManager {
public:
    static bool IsKeyPressed(int key);           // 判断某个键是否被按下
    static bool IsMouseButtonPressed(int button);// 判断某个鼠标按钮是否被按下
    static glm::vec2 GetMousePosition();         // 获取鼠标当前位置
    static float GetMouseScrollOffset();         // 获取鼠标滚轮偏移

    void Update();                               // 更新输入状态
    void RegisterKeyCallback(void (*callback)(int key, int action)); // 注册键盘回调
    void RegisterMouseCallback(void (*callback)(int button, int action)); // 注册鼠标回调
};