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

// ��������
cv::Mat Filter::increaseBrightness(const cv::Mat& mat) {
    // ȷ������ͼ����4ͨ����8λͼ��
    if (mat.empty()) {
        throw std::invalid_argument("����ͼ��Ϊ�գ�");
    }
    if (mat.type() != CV_8UC4) {
        throw std::invalid_argument("����ͼ�������8λ4ͨ��ͼ�� (CV_8UC4)��");
    }
    double beta = static_cast<double>(m_Beta);

    std::vector<cv::Mat> channels;
    cv::split(mat, channels); // B, G, R, A

    // �������Ƚ��� B, G, R ͨ��
    for (int i = 0; i < 3; ++i) {
        channels[i].convertTo(channels[i], -1, 1, beta); // alpha=1, betaΪ��������
    }

    cv::Mat brightMat;
    cv::merge(channels, brightMat); // �ϲ������ĸ�ͨ��
    return brightMat;
}

// �����Աȶ�
cv::Mat Filter::adjustContrast(const cv::Mat& mat) {
    // ȷ������ͼ����4ͨ����8λͼ��
    if (mat.empty()) {
        throw std::invalid_argument("����ͼ��Ϊ�գ�");
    }
    if (mat.type() != CV_8UC4) {
        throw std::invalid_argument("����ͼ�������8λ4ͨ��ͼ�� (CV_8UC4)��");
    }

    std::vector<cv::Mat> channels;
    cv::split(mat, channels); // B, G, R, A

    double alpha = static_cast<double>(m_Alpha);
    // �����ԱȶȽ��� B, G, R ͨ��
    for (int i = 0; i < 3; ++i) {
        channels[i].convertTo(channels[i], -1, alpha, 0); // alphaΪ�Աȶ�����, beta=0
    }

    cv::Mat contrastMat;
    cv::merge(channels, contrastMat); // �ϲ������ĸ�ͨ��
    return contrastMat;
}

// ����ɫ���ͱ��Ͷ�
cv::Mat Filter::changeHueSaturation(const cv::Mat& mat) {
    // ȷ������ͼ����4ͨ����8λͼ��
    if (mat.empty()) {
        throw std::invalid_argument("����ͼ��Ϊ�գ�");
    }
    if (mat.type() != CV_8UC4) {
        throw std::invalid_argument("����ͼ�������8λ4ͨ��ͼ�� (CV_8UC4)��");
    }

    std::vector<cv::Mat> channels;
    cv::split(mat, channels); // B, G, R, A
    double hue = static_cast<double>(m_Hue);
    double saturation = static_cast<double>(m_Saturation);
    // �� BGR ת��Ϊ HSV
    cv::Mat bgr;
    cv::merge(std::vector<cv::Mat>{channels[0], channels[1], channels[2]}, bgr);
    cv::Mat hsv;
    cv::cvtColor(bgr, hsv, cv::COLOR_BGR2HSV);

    std::vector<cv::Mat> hsvChannels;
    cv::split(hsv, hsvChannels); // H, S, V

    // ����ɫ��
    hsvChannels[0].convertTo(hsvChannels[0], CV_32F);
    hsvChannels[0] += hue;
    cv::threshold(hsvChannels[0], hsvChannels[0], 179.0, 179.0, cv::THRESH_TRUNC); // Hue �� OpenCV �з�ΧΪ [0,179]
    cv::threshold(hsvChannels[0], hsvChannels[0], 0.0, 0.0, cv::THRESH_TOZERO);
    hsvChannels[0].convertTo(hsvChannels[0], CV_8U);

    // �������Ͷ�
    hsvChannels[1].convertTo(hsvChannels[1], CV_32F);
    hsvChannels[1] *= saturation;
    cv::threshold(hsvChannels[1], hsvChannels[1], 255.0, 255.0, cv::THRESH_TRUNC); // Saturation �� [0,255]
    cv::threshold(hsvChannels[1], hsvChannels[1], 0.0, 0.0, cv::THRESH_TOZERO);
    hsvChannels[1].convertTo(hsvChannels[1], CV_8U);

    // �ϲ� HSV ��ת���� BGR
    cv::merge(hsvChannels, hsv);
    cv::Mat adjustedBGR;
    cv::cvtColor(hsv, adjustedBGR, cv::COLOR_HSV2BGR);

    // ���������� B, G, R ͨ��
    std::vector<cv::Mat> adjustedChannels;
    cv::split(adjustedBGR, adjustedChannels); // B, G, R

    // ��������� B, G, R ͨ����ԭʼ Alpha ͨ���ϲ�
    channels[0] = adjustedChannels[0];
    channels[1] = adjustedChannels[1];
    channels[2] = adjustedChannels[2];

    cv::Mat adjustedMat;
    cv::merge(channels, adjustedMat); // �ϲ������ĸ�ͨ��
    return adjustedMat;
}

// ��˹ģ��
cv::Mat Filter::applyGaussianBlur(const cv::Mat& mat, int kernelSize) {
    // ȷ������ͼ����4ͨ����8λͼ��
    if (mat.empty()) {
        throw std::invalid_argument("����ͼ��Ϊ�գ�");
    }
    if (mat.type() != CV_8UC4) {
        throw std::invalid_argument("����ͼ�������8λ4ͨ��ͼ�� (CV_8UC4)��");
    }

    // ȷ���˴�СΪ����
    if (kernelSize % 2 == 0) kernelSize += 1;

    // ��˹ģ�����Զ���������ͨ�������� Alpha ͨ��
    cv::Mat blurredMat;
    cv::GaussianBlur(mat, blurredMat, cv::Size(kernelSize, kernelSize), 0);
    return blurredMat;
}

// ���˾�
cv::Mat Filter::applySharpen(const cv::Mat& mat) {
    // ȷ������ͼ����4ͨ����8λͼ��
    if (mat.empty()) {
        throw std::invalid_argument("����ͼ��Ϊ�գ�");
    }
    if (mat.type() != CV_8UC4) {
        throw std::invalid_argument("����ͼ�������8λ4ͨ��ͼ�� (CV_8UC4)��");
    }

    std::vector<cv::Mat> channels;
    cv::split(mat, channels); // B, G, R, A

    // �� B, G, R �ϲ�
    cv::Mat bgr;
    cv::merge(std::vector<cv::Mat>{channels[0], channels[1], channels[2]}, bgr);

    // �����񻯺�
    cv::Mat kernel = (cv::Mat_<float>(3, 3) <<
        0, -1, 0,
        -1, 5, -1,
        0, -1, 0);

    // Ӧ����
    cv::Mat sharpenedBGR;
    cv::filter2D(bgr, sharpenedBGR, bgr.depth(), kernel);

    // �����񻯺�� B, G, R ͨ��
    std::vector<cv::Mat> sharpenedChannels;
    cv::split(sharpenedBGR, sharpenedChannels); // B, G, R

    // ���񻯺�� B, G, R ͨ����ԭʼ Alpha ͨ���ϲ�
    channels[0] = sharpenedChannels[0];
    channels[1] = sharpenedChannels[1];
    channels[2] = sharpenedChannels[2];

    cv::Mat sharpenedMat;
    cv::merge(channels, sharpenedMat); // �ϲ������ĸ�ͨ��
    return sharpenedMat;
}

// �ڰ��˾�
cv::Mat Filter::applyGrayscale(const cv::Mat& mat) {
    // ȷ������ͼ����4ͨ����8λͼ��
    if (mat.empty()) {
        throw std::invalid_argument("����ͼ��Ϊ�գ�");
    }
    if (mat.type() != CV_8UC4) {
        throw std::invalid_argument("����ͼ�������8λ4ͨ��ͼ�� (CV_8UC4)��");
    }

    std::vector<cv::Mat> channels;
    cv::split(mat, channels); // B, G, R, A

    // �� BGR ת��Ϊ�Ҷ�ͼ
    cv::Mat bgr;
    cv::merge(std::vector<cv::Mat>{channels[0], channels[1], channels[2]}, bgr);
    cv::Mat grayMat;
    cv::cvtColor(bgr, grayMat, cv::COLOR_BGR2GRAY);

    // ���Ҷ�ͼת���� BGR
    cv::Mat grayBGR;
    cv::cvtColor(grayMat, grayBGR, cv::COLOR_GRAY2BGR);

    // ����ҶȺ�� B, G, R ͨ��
    std::vector<cv::Mat> grayChannels;
    cv::split(grayBGR, grayChannels); // B, G, R

    // ���ҶȺ�� B, G, R ͨ����ԭʼ Alpha ͨ���ϲ�
    channels[0] = grayChannels[0];
    channels[1] = grayChannels[1];
    channels[2] = grayChannels[2];

    cv::Mat grayImage;
    cv::merge(channels, grayImage); // �ϲ������ĸ�ͨ��
    return grayImage;
}

// �����˾���Sepia��
cv::Mat Filter::applySepia(const cv::Mat& mat) {
    // ȷ������ͼ����4ͨ����8λͼ��
    if (mat.empty()) {
        throw std::invalid_argument("����ͼ��Ϊ�գ�");
    }
    if (mat.type() != CV_8UC4) {
        throw std::invalid_argument("����ͼ�������8λ4ͨ��ͼ�� (CV_8UC4)��");
    }

    std::vector<cv::Mat> channels;
    cv::split(mat, channels); // B, G, R, A

    // �� BGR ת��Ϊ������
    cv::Mat bgr;
    cv::merge(std::vector<cv::Mat>{channels[0], channels[1], channels[2]}, bgr);
    cv::Mat bgrFloat;
    bgr.convertTo(bgrFloat, CV_32F);

    // ���� Sepia �˾���
    cv::Mat kernel = (cv::Mat_<float>(3, 3) <<
        0.272f, 0.534f, 0.131f,
        0.349f, 0.686f, 0.168f,
        0.393f, 0.769f, 0.189f);

    // Ӧ���˾���
    cv::Mat sepiaMatFloat;
    cv::transform(bgrFloat, sepiaMatFloat, kernel);

    // ȷ������ֵ�� [0, 255] ��Χ��
    cv::Mat sepiaMatClipped;
    cv::min(sepiaMatFloat, 255.0f, sepiaMatClipped); // ����255
    cv::max(sepiaMatClipped, 0.0f, sepiaMatClipped);  // ����0

    // ת���� 8 λ�޷�������
    cv::Mat sepiaBGR;
    sepiaMatClipped.convertTo(sepiaBGR, CV_8UC3);

    // ���� Sepia ��� B, G, R ͨ��
    std::vector<cv::Mat> sepiaChannels;
    cv::split(sepiaBGR, sepiaChannels); // B, G, R

    // �� Sepia ��� B, G, R ͨ����ԭʼ Alpha ͨ���ϲ�
    channels[0] = sepiaChannels[0];
    channels[1] = sepiaChannels[1];
    channels[2] = sepiaChannels[2];

    cv::Mat sepiaMat;
    cv::merge(channels, sepiaMat); // �ϲ������ĸ�ͨ��
    return sepiaMat;
}

// ��ɫ�˾�
cv::Mat Filter::applyInvert(const cv::Mat& mat) {
    // ȷ������ͼ����4ͨ����8λͼ��
    if (mat.empty()) {
        throw std::invalid_argument("����ͼ��Ϊ�գ�");
    }
    if (mat.type() != CV_8UC4) {
        throw std::invalid_argument("����ͼ�������8λ4ͨ��ͼ�� (CV_8UC4)��");
    }

    std::vector<cv::Mat> channels;
    cv::split(mat, channels); // B, G, R, A

    // ��ɫ�������� B, G, R ͨ��
    for (int i = 0; i < 3; ++i) {
        cv::bitwise_not(channels[i], channels[i]);
    }

    cv::Mat invertedMat;
    cv::merge(channels, invertedMat); // �ϲ������ĸ�ͨ��
    return invertedMat;
}