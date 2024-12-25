#include "NVIDIA_Nsight.h"
#include <windows.h>
#include <stdio.h>
/*
// ��ȡ����ʵ��
NsightGraphicsManager& NsightGraphicsManager::GetInstance() {
    static NsightGraphicsManager instance;
    return instance;
}

NsightGraphicsManager::NsightGraphicsManager()
    : isInjected_(false), isCapturing_(false) {
}

NsightGraphicsManager::~NsightGraphicsManager() {
    // ��� API �ṩ��ж�ػ�ر�ע��Ĺ��ܣ��������������
    // ���磺NGFX_Injection_Shutdown();
}

bool NsightGraphicsManager::Initialize() {
    std::lock_guard<std::mutex> lock(mtx_);

    if (isInjected_) {
        // �Ѿ���ʼ����ע����������ظ�
        fprintf(stdout, "Nsight Graphics already injected.\n");
        return true;
    }

    NGFX_Injection_Result result;

    // 1. ö�� Nsight Graphics ��װ
    uint32_t installationCount = 0;
    result = NGFX_Injection_EnumerateInstallations(&installationCount, nullptr);
    fprintf(stdout, "EnumerateInstallations returned: %d, count: %d\n", result, installationCount);
    if (result != NGFX_INJECTION_RESULT_OK || installationCount == 0) {
        fprintf(stderr, "No Nsight Graphics installations found or failed to enumerate. Result: %d\n", result);
        return false;
    }

    // �����ڴ沢ö�ٰ�װ��Ϣ
    installations_.resize(installationCount);
    result = NGFX_Injection_EnumerateInstallations(&installationCount, installations_.data());
    fprintf(stdout, "EnumerateInstallations (detail) returned: %d\n", result);
    if (result != NGFX_INJECTION_RESULT_OK) {
        fprintf(stderr, "Failed to enumerate Nsight Graphics installations. Result: %d\n", result);
        installations_.clear();
        return false;
    }

    // ѡ���һ����װ
    chosenInstallation_ = installations_[0];
    fprintf(stdout, "Chosen installation path: %s\n", chosenInstallation_.installationPath);

    // 2. ö����ѡ��װ�Ļ
    uint32_t activityCount = 0;
    result = NGFX_Injection_EnumerateActivities(&chosenInstallation_, &activityCount, nullptr);
    fprintf(stdout, "EnumerateActivities returned: %d, count: %d\n", result, activityCount);
    if (result != NGFX_INJECTION_RESULT_OK || activityCount == 0) {
        fprintf(stderr, "No activities found for the chosen installation or failed to enumerate. Result: %d\n", result);
        installations_.clear();
        return false;
    }

    // �����ڴ沢ö�ٻ��Ϣ
    activities_.resize(activityCount);
    result = NGFX_Injection_EnumerateActivities(&chosenInstallation_, &activityCount, activities_.data());
    fprintf(stdout, "EnumerateActivities (detail) returned: %d\n", result);
    if (result != NGFX_INJECTION_RESULT_OK) {
        fprintf(stderr, "Failed to enumerate activities for the chosen installation. Result: %d\n", result);
        installations_.clear();
        activities_.clear();
        return false;
    }

    // 3. ѡ��֡�������
    bool found = false;
    for (const auto& activity : activities_) {
        if (activity.type == NGFX_INJECTION_ACTIVITY_FRAME_DEBUGGER) {
            chosenActivity_ = activity;
            found = true;
            fprintf(stdout, "Selected activity: %s\n", activity.description);
            break;
        }
    }

    if (!found) {
        fprintf(stderr, "Frame Debugger activity not found in the chosen installation.\n");
        installations_.clear();
        activities_.clear();
        return false;
    }

    // 4. ���û���ã�ʹ�� JSON ���ã�
    const char* jsonConfig = R"({
        "captureSettings": {
            "savePath": "C:\\Path\\To\\Save\\Captures",
            "fileNamePrefix": "capture_",
            "autoCapture": false
        }
    })";

    result = NGFX_Injection_SetActivitySettingsFromProjectJson(&chosenInstallation_, jsonConfig);
    fprintf(stdout, "SetActivitySettingsFromProjectJson returned: %d\n", result);
    if (result != NGFX_INJECTION_RESULT_OK) {
        fprintf(stderr, "Failed to set activity settings from project JSON. Result: %d\n", result);
        // ������Ҫ�����Ƿ����
    }

    // 5. ע�뵽��ǰ����
    result = NGFX_Injection_InjectToProcess(&chosenInstallation_, &chosenActivity_);
    fprintf(stdout, "InjectToProcess returned: %d\n", result);
    if (result != NGFX_INJECTION_RESULT_OK) {
        fprintf(stderr, "Failed to inject Nsight Graphics into the current process. Result: %d\n", result);
        installations_.clear();
        activities_.clear();
        return false;
    }

    isInjected_ = true;
    fprintf(stdout, "Nsight Graphics injected successfully.\n");
    return true;
}

bool NsightGraphicsManager::CaptureFrame() {
    std::lock_guard<std::mutex> lock(mtx_);

    if (!isInjected_) {
        fprintf(stderr, "Nsight Graphics is not injected. Call Initialize() first.\n");
        return false;
    }

    if (isCapturing_) {
        fprintf(stderr, "Frame capture already in progress.\n");
        return false;
    }

    isCapturing_ = true;

    NGFX_Injection_Result result = NGFX_Injection_ExecuteActivityCommand();
    fprintf(stdout, "ExecuteActivityCommand returned: %d\n", result);
    if (result != NGFX_INJECTION_RESULT_OK) {
        fprintf(stderr, "Failed to execute Nsight Graphics frame debugger command. Result: %d\n", result);
        isCapturing_ = false;
        return false;
    }

    fprintf(stdout, "Frame capture command executed successfully.\n");

    isCapturing_ = false;
    return true;
}
NsightGraphicsManager& g_NsightGraphicsManager = NsightGraphicsManager::GetInstance();
*/