#include "Filter.h"
#include <stdexcept>

Filter::Filter()
    :m_Alpha(1.0f), m_Beta(0.0f), m_Hue(1.0f), m_Saturation(1.0f),
    m_ApplyGaussianBlur(false), m_ApplySharpen(false), m_ApplyGrayscale(false),
    m_ApplyInvert(false), m_ApplySepia(false)
{
}

cv::Mat Filter::ApplyFilter(cv::Mat& mat)
{
    cv::Mat matFinal;
    matFinal = increaseBrightness(mat);
    matFinal = adjustContrast(matFinal);
    matFinal = changeHueSaturation(matFinal);
    if (m_ApplyGaussianBlur) {
        matFinal = applyGaussianBlur(matFinal, 5);
    }
    if (m_ApplySharpen) {
        matFinal = applySharpen(matFinal);
    }
    if (m_ApplyGrayscale) {
        matFinal = applyGrayscale(matFinal);
    }
    if (m_ApplyInvert) {
        matFinal = applyInvert(matFinal);
    }
    if (m_ApplySepia) {
        matFinal = applySepia(matFinal);
    }
    return matFinal;
}

// 增加亮度
cv::Mat Filter::increaseBrightness(const cv::Mat& mat) {
    // 确保输入图像是4通道的8位图像
    if (mat.empty()) {
        throw std::invalid_argument("输入图像为空！");
    }
    if (mat.type() != CV_8UC4) {
        throw std::invalid_argument("输入图像必须是8位4通道图像 (CV_8UC4)。");
    }
    double beta = static_cast<double>(m_Beta);

    std::vector<cv::Mat> channels;
    cv::split(mat, channels); // B, G, R, A

    // 增加亮度仅对 B, G, R 通道
    for (int i = 0; i < 3; ++i) {
        channels[i].convertTo(channels[i], -1, 1, beta); // alpha=1, beta为亮度增量
    }

    cv::Mat brightMat;
    cv::merge(channels, brightMat); // 合并所有四个通道
    return brightMat;
}

// 调整对比度
cv::Mat Filter::adjustContrast(const cv::Mat& mat) {
    // 确保输入图像是4通道的8位图像
    if (mat.empty()) {
        throw std::invalid_argument("输入图像为空！");
    }
    if (mat.type() != CV_8UC4) {
        throw std::invalid_argument("输入图像必须是8位4通道图像 (CV_8UC4)。");
    }

    std::vector<cv::Mat> channels;
    cv::split(mat, channels); // B, G, R, A

    double alpha = static_cast<double>(m_Alpha);
    // 调整对比度仅对 B, G, R 通道
    for (int i = 0; i < 3; ++i) {
        channels[i].convertTo(channels[i], -1, alpha, 0); // alpha为对比度因子, beta=0
    }

    cv::Mat contrastMat;
    cv::merge(channels, contrastMat); // 合并所有四个通道
    return contrastMat;
}

// 调整色调和饱和度
cv::Mat Filter::changeHueSaturation(const cv::Mat& mat) {
    // 确保输入图像是4通道的8位图像
    if (mat.empty()) {
        throw std::invalid_argument("输入图像为空！");
    }
    if (mat.type() != CV_8UC4) {
        throw std::invalid_argument("输入图像必须是8位4通道图像 (CV_8UC4)。");
    }

    std::vector<cv::Mat> channels;
    cv::split(mat, channels); // B, G, R, A
    double hue = static_cast<double>(m_Hue);
    double saturation = static_cast<double>(m_Saturation);
    // 将 BGR 转换为 HSV
    cv::Mat bgr;
    cv::merge(std::vector<cv::Mat>{channels[0], channels[1], channels[2]}, bgr);
    cv::Mat hsv;
    cv::cvtColor(bgr, hsv, cv::COLOR_BGR2HSV);

    std::vector<cv::Mat> hsvChannels;
    cv::split(hsv, hsvChannels); // H, S, V

    // 调整色调
    hsvChannels[0].convertTo(hsvChannels[0], CV_32F);
    hsvChannels[0] += hue;
    cv::threshold(hsvChannels[0], hsvChannels[0], 179.0, 179.0, cv::THRESH_TRUNC); // Hue 在 OpenCV 中范围为 [0,179]
    cv::threshold(hsvChannels[0], hsvChannels[0], 0.0, 0.0, cv::THRESH_TOZERO);
    hsvChannels[0].convertTo(hsvChannels[0], CV_8U);

    // 调整饱和度
    hsvChannels[1].convertTo(hsvChannels[1], CV_32F);
    hsvChannels[1] *= saturation;
    cv::threshold(hsvChannels[1], hsvChannels[1], 255.0, 255.0, cv::THRESH_TRUNC); // Saturation 在 [0,255]
    cv::threshold(hsvChannels[1], hsvChannels[1], 0.0, 0.0, cv::THRESH_TOZERO);
    hsvChannels[1].convertTo(hsvChannels[1], CV_8U);

    // 合并 HSV 并转换回 BGR
    cv::merge(hsvChannels, hsv);
    cv::Mat adjustedBGR;
    cv::cvtColor(hsv, adjustedBGR, cv::COLOR_HSV2BGR);

    // 分离调整后的 B, G, R 通道
    std::vector<cv::Mat> adjustedChannels;
    cv::split(adjustedBGR, adjustedChannels); // B, G, R

    // 将调整后的 B, G, R 通道与原始 Alpha 通道合并
    channels[0] = adjustedChannels[0];
    channels[1] = adjustedChannels[1];
    channels[2] = adjustedChannels[2];

    cv::Mat adjustedMat;
    cv::merge(channels, adjustedMat); // 合并所有四个通道
    return adjustedMat;
}

// 高斯模糊
cv::Mat Filter::applyGaussianBlur(const cv::Mat& mat, int kernelSize) {
    // 确保输入图像是4通道的8位图像
    if (mat.empty()) {
        throw std::invalid_argument("输入图像为空！");
    }
    if (mat.type() != CV_8UC4) {
        throw std::invalid_argument("输入图像必须是8位4通道图像 (CV_8UC4)。");
    }

    // 确保核大小为奇数
    if (kernelSize % 2 == 0) kernelSize += 1;

    // 高斯模糊会自动处理所有通道，包括 Alpha 通道
    cv::Mat blurredMat;
    cv::GaussianBlur(mat, blurredMat, cv::Size(kernelSize, kernelSize), 0);
    return blurredMat;
}

// 锐化滤镜
cv::Mat Filter::applySharpen(const cv::Mat& mat) {
    // 确保输入图像是4通道的8位图像
    if (mat.empty()) {
        throw std::invalid_argument("输入图像为空！");
    }
    if (mat.type() != CV_8UC4) {
        throw std::invalid_argument("输入图像必须是8位4通道图像 (CV_8UC4)。");
    }

    std::vector<cv::Mat> channels;
    cv::split(mat, channels); // B, G, R, A

    // 将 B, G, R 合并
    cv::Mat bgr;
    cv::merge(std::vector<cv::Mat>{channels[0], channels[1], channels[2]}, bgr);

    // 定义锐化核
    cv::Mat kernel = (cv::Mat_<float>(3, 3) <<
        0, -1, 0,
        -1, 5, -1,
        0, -1, 0);

    // 应用锐化
    cv::Mat sharpenedBGR;
    cv::filter2D(bgr, sharpenedBGR, bgr.depth(), kernel);

    // 分离锐化后的 B, G, R 通道
    std::vector<cv::Mat> sharpenedChannels;
    cv::split(sharpenedBGR, sharpenedChannels); // B, G, R

    // 将锐化后的 B, G, R 通道与原始 Alpha 通道合并
    channels[0] = sharpenedChannels[0];
    channels[1] = sharpenedChannels[1];
    channels[2] = sharpenedChannels[2];

    cv::Mat sharpenedMat;
    cv::merge(channels, sharpenedMat); // 合并所有四个通道
    return sharpenedMat;
}

// 黑白滤镜
cv::Mat Filter::applyGrayscale(const cv::Mat& mat) {
    // 确保输入图像是4通道的8位图像
    if (mat.empty()) {
        throw std::invalid_argument("输入图像为空！");
    }
    if (mat.type() != CV_8UC4) {
        throw std::invalid_argument("输入图像必须是8位4通道图像 (CV_8UC4)。");
    }

    std::vector<cv::Mat> channels;
    cv::split(mat, channels); // B, G, R, A

    // 将 BGR 转换为灰度图
    cv::Mat bgr;
    cv::merge(std::vector<cv::Mat>{channels[0], channels[1], channels[2]}, bgr);
    cv::Mat grayMat;
    cv::cvtColor(bgr, grayMat, cv::COLOR_BGR2GRAY);

    // 将灰度图转换回 BGR
    cv::Mat grayBGR;
    cv::cvtColor(grayMat, grayBGR, cv::COLOR_GRAY2BGR);

    // 分离灰度后的 B, G, R 通道
    std::vector<cv::Mat> grayChannels;
    cv::split(grayBGR, grayChannels); // B, G, R

    // 将灰度后的 B, G, R 通道与原始 Alpha 通道合并
    channels[0] = grayChannels[0];
    channels[1] = grayChannels[1];
    channels[2] = grayChannels[2];

    cv::Mat grayImage;
    cv::merge(channels, grayImage); // 合并所有四个通道
    return grayImage;
}

// 怀旧滤镜（Sepia）
cv::Mat Filter::applySepia(const cv::Mat& mat) {
    // 确保输入图像是4通道的8位图像
    if (mat.empty()) {
        throw std::invalid_argument("输入图像为空！");
    }
    if (mat.type() != CV_8UC4) {
        throw std::invalid_argument("输入图像必须是8位4通道图像 (CV_8UC4)。");
    }

    std::vector<cv::Mat> channels;
    cv::split(mat, channels); // B, G, R, A

    // 将 BGR 转换为浮点型
    cv::Mat bgr;
    cv::merge(std::vector<cv::Mat>{channels[0], channels[1], channels[2]}, bgr);
    cv::Mat bgrFloat;
    bgr.convertTo(bgrFloat, CV_32F);

    // 定义 Sepia 滤镜核
    cv::Mat kernel = (cv::Mat_<float>(3, 3) <<
        0.272f, 0.534f, 0.131f,
        0.349f, 0.686f, 0.168f,
        0.393f, 0.769f, 0.189f);

    // 应用滤镜核
    cv::Mat sepiaMatFloat;
    cv::transform(bgrFloat, sepiaMatFloat, kernel);

    // 确保像素值在 [0, 255] 范围内
    cv::Mat sepiaMatClipped;
    cv::min(sepiaMatFloat, 255.0f, sepiaMatClipped); // 上限255
    cv::max(sepiaMatClipped, 0.0f, sepiaMatClipped);  // 下限0

    // 转换回 8 位无符号整数
    cv::Mat sepiaBGR;
    sepiaMatClipped.convertTo(sepiaBGR, CV_8UC3);

    // 分离 Sepia 后的 B, G, R 通道
    std::vector<cv::Mat> sepiaChannels;
    cv::split(sepiaBGR, sepiaChannels); // B, G, R

    // 将 Sepia 后的 B, G, R 通道与原始 Alpha 通道合并
    channels[0] = sepiaChannels[0];
    channels[1] = sepiaChannels[1];
    channels[2] = sepiaChannels[2];

    cv::Mat sepiaMat;
    cv::merge(channels, sepiaMat); // 合并所有四个通道
    return sepiaMat;
}

// 反色滤镜
cv::Mat Filter::applyInvert(const cv::Mat& mat) {
    // 确保输入图像是4通道的8位图像
    if (mat.empty()) {
        throw std::invalid_argument("输入图像为空！");
    }
    if (mat.type() != CV_8UC4) {
        throw std::invalid_argument("输入图像必须是8位4通道图像 (CV_8UC4)。");
    }

    std::vector<cv::Mat> channels;
    cv::split(mat, channels); // B, G, R, A

    // 反色操作仅对 B, G, R 通道
    for (int i = 0; i < 3; ++i) {
        cv::bitwise_not(channels[i], channels[i]);
    }

    cv::Mat invertedMat;
    cv::merge(channels, invertedMat); // 合并所有四个通道
    return invertedMat;
}