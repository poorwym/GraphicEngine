#pragma once
#include "Entity.h"
class AnimationSystem {
public:
    void Update(float deltaTime);                 // ���¶���ϵͳ
    void PlayAnimation(Entity* entity, const std::string& animationName);
};

class SkeletalAnimation {
public:
    void Apply(Entity* entity, float time);       // ����������Ӧ�õ�ʵ����
};