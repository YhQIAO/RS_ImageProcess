#include<iostream>
#include<opencv2/opencv.hpp>
#include<Eigen/core>

#include "MyImageProcessLib.h"
#include "Brovey.h"
#include "PCA_Fusion.h"
#include "HSI_Fusion.h"

using namespace std;

int main() {
	string multiBandPath = "../../Chap8/data/Landsat8_OLI_multi.dat";
	string highImagePath = "../../Chap8/data/Landsat8_OLI_pan.dat";


	// 演示Brovey图像融合
	Brovey();

	// 演示PCA
	vector<cv::Mat> multiBandImage = readMultiSpectralImage(
		multiBandPath, 397, 395, 7, 2, "BIL");
	cv::Mat highImage = readBinaryImageOfBandX(
		highImagePath, 796, 792, 1, 1, 12, "BSQ");
	PCA_Fusion(multiBandImage, highImage);

	// 演示HSI融合
	cv::Mat mul[] = { multiBandImage.at(1),
		multiBandImage.at(2),multiBandImage.at(3) };
	cv::Mat multiBGRImage(multiBandImage.at(1).size(), CV_8UC3);
	cv::merge(mul, 3, multiBGRImage);

	HSI_Fusion(multiBGRImage, highImage);
	cv::imshow("multiBGRImage", multiBGRImage);
	cv::waitKey(0);
}
