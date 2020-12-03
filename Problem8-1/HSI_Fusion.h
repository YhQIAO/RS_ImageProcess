#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <Eigen/Dense>
#include <Eigen/Core>
#include <opencv2/core/eigen.hpp>
#include "MyImageProcessLib.h"

using namespace std;
using namespace Eigen;


void HSI_Fusion(cv::Mat multiBGRImage, cv::Mat highImage) {

	cv::Mat multiHSIImage = BGR2HSI(multiBGRImage);

	cv::Mat highHSIImage(highImage.size(), CV_32FC3);
	for (int i = 0; i < highHSIImage.rows; i++) {
		for (int j = 0; j < highHSIImage.cols; j++) {
			
			// 重采样
			int m = (int)(((double)i / highHSIImage.rows) * multiHSIImage.rows);
			int n = (int)(((double)j / highHSIImage.cols) * multiHSIImage.cols);

			// 赋值H,S
			highHSIImage.at<cv::Vec3f>(i, j)[0] = multiHSIImage.at<cv::Vec3f>(m, n)[0];
			highHSIImage.at<cv::Vec3f>(i, j)[1] = multiHSIImage.at<cv::Vec3f>(m, n)[1];
			// 赋值I
			highHSIImage.at<cv::Vec3f>(i, j)[2] = highImage.at<uchar>(i, j) / 255.0;
		}
	}
	cv::Mat result = HSI2BGR(highHSIImage);
	cv::imshow("result", result);
}