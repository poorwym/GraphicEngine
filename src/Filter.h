#pragma once
#include "opencv2/opencv.hpp"

//把filter改成滤镜的名字就行。
class Filter {
public:
	cv::Mat Filter1(cv::Mat& mat);
	//增加不同的滤镜
};