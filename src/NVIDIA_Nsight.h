#pragma once
/*
#include "NGFX_Injection.h"
#include <vector>
#include <mutex>

class NsightGraphicsManager {
public:
    // ��ȡ����ʵ��
    static NsightGraphicsManager& GetInstance();

    // ��ʼ�� Nsight Graphics ע��
    bool Initialize();

    // ����ǰ֡
    bool CaptureFrame();

private:
    // ˽�й��캯������ֹ�ⲿʵ����
    NsightGraphicsManager();
    ~NsightGraphicsManager();

    // ��ֹ�����͸�ֵ
    NsightGraphicsManager(const NsightGraphicsManager&) = delete;
    NsightGraphicsManager& operator=(const NsightGraphicsManager&) = delete;

    // �洢ö�ٵ��İ�װ��Ϣ
    std::vector<NGFX_Injection_InstallationInfo> installations_;

    // �洢ö�ٵ��Ļ��Ϣ
    std::vector<NGFX_Injection_Activity> activities_;

    // ѡ��İ�װ�ͻ
    NGFX_Injection_InstallationInfo chosenInstallation_;
    NGFX_Injection_Activity chosenActivity_;

    // ע��״̬
    bool isInjected_;

    // ����״̬
    bool isCapturing_;

    // ��������ȷ���̰߳�ȫ
    std::mutex mtx_;
};
*/