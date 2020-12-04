//
// Created by 乔亦弘 on 2020/12/4.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include "MyImageProcessLib.h"
#include "Histogram.h"

using namespace std;

int main() {
    string filepath = "../../Chap8/data/Landsat8_OLI_b1.dat";
    cv::Mat oriImage = readBinaryImageOfBandX(
            filepath,397,395,1,1,1,"BSQ");
    cv::imshow("oriImage", oriImage);
    cv::imshow("ori_his", getHistogramPic(oriImage));
    cv::Mat histogramNormalized = HistogramNormalize(oriImage);
    cv::imshow("HistogramNormalize", histogramNormalized);
    cv::imshow("HistogramNormalize_his", getHistogramPic(histogramNormalized));
    cv::waitKey(0);
}
