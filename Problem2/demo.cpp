#include<iostream>
#include<fstream>
#include <string>
#include<opencv2/opencv.hpp>
#include"getHistoGraoh.h"

using namespace std;
using namespace cv;


int main() {

	string outFilePath = R"+*(..\..\第2章_数字图像存储\Code\NDVI_output\NDVI_image.dat)+*";
	string binaryPath = R"+*(..\..\\第2章_数字图像存储\data\binary_image)+*";


	/*
	列数：515
	行数：449
	波段数：4
	文件类型：ENVI标准格式
	字节序：小端（Host Intel）
	数据类型：Float
	多波段存储方式：BIL
	*/

	Mat binaryImage(449, 515, CV_8UC3);
	int originalData[449][515];


	int num = 0;
	ifstream infile(binaryPath, ios::in | ios::binary);
	if (!infile) {
		cout << "error!";
		return 0;
	}

	float value;
	infile.seekg(22, ios::cur); //跳过开头的22个字节
	while (infile.read(
		(char*)&value, sizeof(value)
			)
		) {
		if (num > 515 * 449 * 4 - 1) {
			break;
		}
		int line = num / 515 / 4;
		int band = (num / 515) % 4;
		int col = num % 515;

		num++;

		if (band < 3) {
			binaryImage.at<Vec3b>(line, col)[band] = (int)value/8;
		}
	}
	infile.close();

	/*imshow("showImage", binaryImage);
	waitKey(0);*/


	// 统计直方图
	Mat grayImage;
	cvtColor(binaryImage, grayImage, CV_BGR2GRAY);

	/*imshow("grayImage", grayImage);
	waitKey(0);*/

	getHistograph(grayImage);


}