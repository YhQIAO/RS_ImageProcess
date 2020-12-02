#pragma once

#include<iostream>
#include<opencv2/opencv.hpp>
#include<Eigen/core>

#include "MyImageProcessLib.h"

using namespace std;

void Brovey() {
	string multiBandPath = "../../Chap8/data/Landsat8_OLI_multi.dat";
	cv::Mat multiBand[] = {
		readBinaryImageOfBandX(multiBandPath, 397, 395, 2, 7, 2, "BIL"),
		readBinaryImageOfBandX(multiBandPath, 397, 395, 3, 7, 2, "BIL"),
		readBinaryImageOfBandX(multiBandPath, 397, 395, 4, 7, 2, "BIL")
	};
	cv::Mat multiBGRImage;
	cv::merge(multiBand, 3, multiBGRImage);


	string panPath = "../../Chap8/data/Landsat8_OLI_pan.dat";
	cv::Mat panImage = readBinaryImageOfBandX(panPath, 796, 792, 1, 1, 12, "BSQ");
    ;
	cv::Mat panBGR[] = {
		cv::Mat(panImage.size(), CV_8UC1),
		cv::Mat(panImage.size(), CV_8UC1),
		cv::Mat(panImage.size(), CV_8UC1)
	};

	for (int i = 0; i < panImage.rows; i++) {
		for (int j = 0; j < panImage.cols; j++) {

			int m = (int)(((double)i / panImage.rows) * multiBGRImage.rows);
			int n = (int)(((double)j / panImage.cols) * multiBGRImage.cols);


			int sum = (int)multiBand[0].at<uchar>(m, n) +
                    (int)multiBand[1].at<uchar>(m, n) +
                    (int)multiBand[2].at<uchar>(m, n);

			for (int k = 0; k < 3; k++) {
				panBGR[k].at<uchar>(i, j) = 3*(int)(
					(double)(panImage.at<uchar>(i, j) * multiBand[k].at<uchar>(m, n)) / sum
					);
			}
		}
	}

	cv::Mat panBGRImage;
	cv::merge(panBGR, 3, panBGRImage);


	cv::imshow("multiImage", multiBGRImage);
	cv::imshow("panImage", panImage);
	cv::imshow("panBGRImage", panBGRImage);
	cv::waitKey(0);

}