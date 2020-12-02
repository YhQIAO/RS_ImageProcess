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
	cv::merge(multiBand, 3, multiBGRImage);  // CV_16S


	string panPath = "../../Chap8/data/Landsat8_OLI_pan.dat";
	cv::Mat panImage = readBinaryImageOfBandX(panPath, 796, 792, 1, 1, 12, "BSQ");

	cv::Mat panBGR[] = {
		cv::Mat(panImage.size(), CV_16UC1),
		cv::Mat(panImage.size(), CV_16UC1),
		cv::Mat(panImage.size(), CV_16UC1)
	};

	for (int i = 0; i < panImage.rows; i++) {
		for (int j = 0; j < panImage.cols; j++) {

			int m = (int)(((double)i / panImage.rows) * multiBGRImage.rows);
			int n = (int)(((double)j / panImage.cols) * multiBGRImage.cols);


			int sum = multiBand[0].at<int16_t>(m, n) +
				multiBand[1].at<int16_t>(m, n) +
				multiBand[2].at<int16_t>(m, n);

			for (int k = 0; k < 3; k++) {
				panBGR[k].at<uint16_t>(i, j) = (int)(
					(double)(panImage.at<uint16_t>(i, j) * multiBand[k].at<int16_t>(m, n)) / sum
					);
				// cout << panBGR[k].at<int16_t>(i, j) << endl;
			}
		}
	}

	cv::Mat panBGRImage;
	cv::merge(panBGR, 3, panBGRImage);


	cv::imshow("multiImage", multiBGRImage *20);
	cv::imshow("panImage", panImage * 4);
	cv::imshow("panBGRImage", panBGRImage * 10);
	cv::waitKey(0);

}