#pragma once
#include "glm/glm.hpp"

class InputManager {
public:
    static bool IsKeyPressed(int key);           // �ж�ĳ�����Ƿ񱻰���
    static bool IsMouseButtonPressed(int button);// �ж�ĳ����갴ť�Ƿ񱻰���
    static glm::vec2 GetMousePosition();         // ��ȡ��굱ǰλ��
    static float GetMouseScrollOffset();         // ��ȡ������ƫ��

    void Update();                               // ��������״̬
    void RegisterKeyCallback(void (*callback)(int key, int action)); // ע����̻ص�
    void RegisterMouseCallback(void (*callback)(int button, int action)); // ע�����ص�
};