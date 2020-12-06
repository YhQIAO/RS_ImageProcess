#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>

#include "Histogram.h"

using namespace std;

cv::Mat readBinary8bitImage(
	string filepath, int rowNum, int colNum);

int main() {
	string filepath = "../../Chap8/data/Landsat8_OLI_b1.dat";

	// 显示原始图像与直方图
	cv::Mat oriImage = readBinary8bitImage(filepath, 397, 395);
	//cv::imshow("oriImage", oriImage);
	//cv::imshow("ori_his", getHistogramPic(oriImage));

	//// 显示直方图均衡化后的图像与直方图
	//cv::Mat histogramNormalized = HistogramNormalize(oriImage);
	//cv::imshow("HistogramNormalize", histogramNormalized);
	//cv::imshow("HistogramNormalize_his", getHistogramPic(histogramNormalized));
	
	// 读取一个灰度图用作匹配图（经典lena）
	
	// 来随便构造一个奇奇怪怪的直方图分布
	double his[256] = { 0 };
	for (int i = 0; i < 256; i++) {
		double y1 = exp(-((i - 207) * (i - 207)) / (double)(2 * 20 * 20));
		double y2 = exp(-((i - 47) * (i - 47)) / (double)(2 * 20 * 20));
		his[i] = y1 + y2;
	}

	cv::Mat lena = cv::imread("../../Chap8/Data/lena.jpg",-1);
	showArrHis(his);
	cv::imshow("n", HistogramNormalize(lena));
	cv::Mat match = hisMatch(lena, his);
	cv::imshow("match", match);
	cv::imshow("match_his", getHistogramPic(match));
	cv::waitKey(0);
}


cv::Mat readBinary8bitImage(
	string filepath, int rowNum, int colNum) {

	cv::Mat image(rowNum, colNum, CV_8UC1);
	int num = 0;
	int value;
	ifstream readDatFstream(filepath, ios::binary | ios::in);
	while (readDatFstream.read(
		(char*)&value, 1
	)) {
		int row = (num / colNum);
		int col = num % colNum;
		image.at<uchar>(row, col) = value;
		num++;
	}
	return image;
}
