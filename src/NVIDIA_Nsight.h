#pragma once
/*
#include "NGFX_Injection.h"
#include <vector>
#include <mutex>

class NsightGraphicsManager {
public:
    // 获取单例实例
    static NsightGraphicsManager& GetInstance();

    // 初始化 Nsight Graphics 注入
    bool Initialize();

    // 捕获当前帧
    bool CaptureFrame();

private:
    // 私有构造函数，防止外部实例化
    NsightGraphicsManager();
    ~NsightGraphicsManager();

    // 禁止拷贝和赋值
    NsightGraphicsManager(const NsightGraphicsManager&) = delete;
    NsightGraphicsManager& operator=(const NsightGraphicsManager&) = delete;

    // 存储枚举到的安装信息
    std::vector<NGFX_Injection_InstallationInfo> installations_;

    // 存储枚举到的活动信息
    std::vector<NGFX_Injection_Activity> activities_;

    // 选择的安装和活动
    NGFX_Injection_InstallationInfo chosenInstallation_;
    NGFX_Injection_Activity chosenActivity_;

    // 注入状态
    bool isInjected_;

    // 捕获状态
    bool isCapturing_;

    // 互斥锁，确保线程安全
    std::mutex mtx_;
};
*/