#include <iostream>
#include <complex>
#include <opencv2/opencv.hpp>
#include <Eigen/Dense>
#include <Eigen/Core>
#include <opencv2/core/eigen.hpp>
#include "DiscreteFourierTransform.h"
#include "MyImageProcessLib.h"
#include "ColorTransFormation.h"

using namespace std;


int main() {

    string filepath = "../第4章_变换域处理方法/data4/Landsat8_OLI_multi.dat";
    cv::Mat bandData = readBinaryImageOfBandX(filepath, 394, 390, 2, 7, 2);

    cv::Mat result =DFT(bandData);
    cv::imshow("result", result);
    cv::waitKey(0);


//    // 读取rgb波段图片
//    cv::Mat redBand = readBinaryImageOfBandX(filepath, 394, 390, 4, 7, 2);
//    cv::Mat greenBand = readBinaryImageOfBandX(filepath, 394, 390, 3, 7, 2);
//    cv::Mat blueBand = readBinaryImageOfBandX(filepath, 394, 390, 2, 7, 2);
//
//    Matrix<int, Dynamic, Dynamic> redData(394, 390);
//    Matrix<int, Dynamic, Dynamic> greenData(394, 390);
//    Matrix<int, Dynamic, Dynamic> blueData(394, 390);
//
//    cv::cv2eigen(redBand, redData);
//    cv::cv2eigen(greenBand, greenData);
//    cv::cv2eigen(blueBand, blueData);
//
//    Matrix<int, Dynamic, Dynamic>::Index a, b;
//    double min = redData.minCoeff(&a, &b);
//    double max = redData.maxCoeff(&a, &b);
//    Matrix<int, Dynamic, Dynamic> NormalRedData = (redData.cast<float>() * (255 / (float)(max - min))).cast<int>();
//
//    min = greenData.minCoeff(&a, &b);
//    max = greenData.maxCoeff(&a, &b);
//    Matrix<int, Dynamic, Dynamic> NormalGreenData = (greenData.cast<float>() * (255 / (float)(max - min))).cast<int>();
//
//    min = blueData.minCoeff(&a, &b);
//    max = blueData.maxCoeff(&a, &b);
//    Matrix<int, Dynamic, Dynamic> NormalBlueData = (blueData.cast<float>() * (255 / (float)(max - min))).cast<int>();
//
//    cv::Mat rgbImage(394, 390, CV_8UC3);
//    for (int i = 0; i < 394; i++) {
//        for (int j = 0; j < 390; j++) {
//            rgbImage.at<cv::Vec3b>(i, j) = cv::Vec3b(NormalBlueData(i, j),
//                                                     NormalGreenData(i, j), NormalRedData(i, j));
//        }
//    }
//
//    cv::imshow("rgbImage", rgbImage);
//    cv::waitKey(0);
//
//
//    cv::Mat cmykImage = BGR2CMYK(rgbImage);
//    cv::imshow("cmykImage", cmykImage);
//    cv::waitKey(0);
//
//    cv::Mat HSIImage(394, 390, CV_8UC3);
//    rgb2hsi(rgbImage, HSIImage);
//    cv::imshow("HsiImage", HSIImage);
//    cv::waitKey(0);


}