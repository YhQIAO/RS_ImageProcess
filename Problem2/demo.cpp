#include<iostream>
#include<fstream>
#include <string>
#include<opencv2/opencv.hpp>
#include"getHistoGraoh.h"

using namespace std;
using namespace cv;


int main() {

	string outFilePath = R"+*(..\..\��2��_����ͼ��洢\Code\NDVI_output\NDVI_image.dat)+*";
	string binaryPath = R"+*(..\..\\��2��_����ͼ��洢\data\binary_image)+*";


	/*
	������515
	������449
	��������4
	�ļ����ͣ�ENVI��׼��ʽ
	�ֽ���С�ˣ�Host Intel��
	�������ͣ�Float
	�ನ�δ洢��ʽ��BIL
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
	infile.seekg(22, ios::cur); //������ͷ��22���ֽ�
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


	// ͳ��ֱ��ͼ
	Mat grayImage;
	cvtColor(binaryImage, grayImage, CV_BGR2GRAY);

	/*imshow("grayImage", grayImage);
	waitKey(0);*/

	getHistograph(grayImage);


}