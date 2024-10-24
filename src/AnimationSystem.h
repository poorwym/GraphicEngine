#pragma once
#include "Entity.h"
class AnimationSystem {
public:
    void Update(float deltaTime);                 // 更新动画系统
    void PlayAnimation(Entity* entity, const std::string& animationName);
};

class SkeletalAnimation {
public:
    void Apply(Entity* entity, float time);       // 将骨骼动画应用到实体上
};