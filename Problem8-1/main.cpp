#include<iostream>
#include<opencv2/opencv.hpp>
#include<Eigen/core>

#include "MyImageProcessLib.h"
#include "Brovey.h"
#include "PCA_Fusion.h"

using namespace std;

int main() {
    string multiBandPath = "../../Chap8/data/Landsat8_OLI_multi.dat";
    // 演示Brovey图像融合
	// Brovey();

	// 演示PCA
	vector<cv::Mat> multiBandImage = readMultiSpectralImage(
	        multiBandPath,397, 395, 7, 2,"BIL");
    string highImagePath = "../../Chap8/data/Landsat8_OLI_pan.dat";
    cv::Mat highImage = readBinaryImageOfBandX(
            highImagePath, 796, 792, 1, 1, 12, "BSQ");
	PCA_Fusion(multiBandImage, highImage);

}