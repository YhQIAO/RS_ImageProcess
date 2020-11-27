#include<iostream>
#include<fstream>
#include"MyFuncs.h"
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main() {

	string filepath = "..\\..\\µÚ3ÕÂ_¿ÕÓò´¦Àí\\data\\Landsat8_OLI_b1.dat";

	Mat oriImage = readBinaryImage(filepath, 397, 395, 1, 1);

	/*imshow("oriImage", oriImage);
	waitKey(0);*/

	Mat Kernel(5, 5, CV_8UC1);
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			Kernel.at<uchar>(i, j) = 1;
		}
	}

	Mat erodedImage = erosionGrayImage(oriImage, Kernel);
	Mat dilatedImage = dilateGrayImage(oriImage, Kernel);
	Mat OpenedImage = OpenGrayImage(oriImage, Kernel);
	Mat closedImage = CloseGrayImage(oriImage, Kernel);


	imshow("original_image", oriImage);
	waitKey(0);

	imshow("erodedImage", erodedImage);
	waitKey(0);

	imshow("dilatedImage", dilatedImage);
	waitKey(0);

	imshow("OpenedImage", OpenedImage);
	waitKey(0);

	imshow("closedImage", closedImage);
	waitKey(0);

}