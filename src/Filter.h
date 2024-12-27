#pragma once
#include "opencv2/opencv.hpp"

class Filter {
public:
    float m_Alpha;
    float m_Beta;
    float m_Hue;
    float m_Saturation;

    bool m_ApplyGaussianBlur;
    bool m_ApplySharpen;
    bool m_ApplyGrayscale;
    bool m_ApplySepia;
    bool m_ApplyInvert;
private:
    // ��ɫ�˾�
    cv::Mat increaseBrightness(const cv::Mat& mat); // ��������
    cv::Mat adjustContrast(const cv::Mat& mat);     // �����Աȶ�
    cv::Mat changeHueSaturation(const cv::Mat& mat); // ����ɫ���ͱ��Ͷ�

    // ��˹ģ��
    cv::Mat applyGaussianBlur(const cv::Mat& mat, int kernelSize = 5);

    // ���˾�
    cv::Mat applySharpen(const cv::Mat& mat);

    // �ڰ��˾�
    cv::Mat applyGrayscale(const cv::Mat& mat);

    // ���������˾�
    cv::Mat applySepia(const cv::Mat& mat); // �����˾�
    cv::Mat applyInvert(const cv::Mat& mat); // ��ɫ�˾�
public:
    Filter();
    cv::Mat ApplyFilter(cv::Mat& mat);
};