//
// Created by 乔亦弘 on 2020/12/8.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>
#include "ImageEnhance.h"

using namespace std;

cv::Mat readBinary8bitImage(string filepath, int rowNum, int colNum);
using namespace std;

int main(){
    string filepath = "../../Chap8/data/Landsat8_OLI_b2.dat";
    cv::Mat oriImage = readBinary8bitImage(filepath, 397,395);
    cv::imshow("oriImage", oriImage);
    // 反锐化掩膜图像增强
    cv::Mat robert_edge = Robert(oriImage);
    cv::imshow("robert_edge", robert_edge);
    cv::imshow("robert_enhance", oriImage+robert_edge);
    cv::waitKey(0);
    // sobel 微分算子图像增强
    cv::Mat sobel_edge = Sobel(oriImage);
    cv::imshow("sobel_edge",sobel_edge);
    cv::imshow("sobel_enhance",oriImage+sobel_edge);
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

