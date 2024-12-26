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
    // 调色滤镜
    cv::Mat increaseBrightness(const cv::Mat& mat); // 增加亮度
    cv::Mat adjustContrast(const cv::Mat& mat);     // 调整对比度
    cv::Mat changeHueSaturation(const cv::Mat& mat); // 调整色调和饱和度

    // 高斯模糊
    cv::Mat applyGaussianBlur(const cv::Mat& mat, int kernelSize = 5);

    // 锐化滤镜
    cv::Mat applySharpen(const cv::Mat& mat);

    // 黑白滤镜
    cv::Mat applyGrayscale(const cv::Mat& mat);

    // 其他常见滤镜
    cv::Mat applySepia(const cv::Mat& mat); // 怀旧滤镜
    cv::Mat applyInvert(const cv::Mat& mat); // 反色滤镜
public:
    Filter();
    cv::Mat ApplyFilter(cv::Mat& mat);
};