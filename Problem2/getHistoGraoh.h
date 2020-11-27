#pragma once
#include<opencv2/opencv.hpp>
#include<iostream>
using namespace cv;
using namespace std;


int getMax(int a[], int n) {
	int max = a[0];
	for (int i = 1; i < n; i++) {
		if (a[i] > max)
		{
			max = a[i];
		}
	}
	return max;
}

void getHistograph(Mat grayImage)
{
	int nRow = grayImage.size[0];
	int nCol = grayImage.size[1];
	int his[256] = { 0 };
	for (int i = 0; i < nRow; i++) {
		for (int j = 0; j < nCol; j++) {
			int value = grayImage.at<uchar>(i, j);
			his[value]++;
		}
	}

	/*for (int i = 0; i < 255; i++) {
		cout << his[i] << endl;
	}*/

	// 绘制灰度图像

	Mat hisGraph(256, 256, CV_8UC1);
	/*rectangle(hisGraph, Rect(100, 20, 20, 20), Scalar(0, 0, 0), -1);*/

	//  每一个像元绘制值绘制一个小矩形
	int max = getMax(his, 256);
	for (int i = 0; i < 255; i++) {
		int height = (int) (((double)his[i] / max) * 256);
		rectangle(hisGraph, Rect(i, 256 - height, 1, height), Scalar(0, 0, 0), -1);
	}



	imshow("hisGraph", hisGraph);
	waitKey(0);
}



