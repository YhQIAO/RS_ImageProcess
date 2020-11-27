#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <Eigen/Dense>
#include <Eigen/Core>
#include<opencv2/core/eigen.hpp>

using namespace std;
using namespace Eigen;

cv::Mat readBinaryImageOfBandX(
    string filepath, int rowNum, int colNum,
    int bandNum, int AllBandNum, int dataType) {

    if (dataType == 2) {
        cv::Mat image(rowNum, colNum, CV_16SC1);
        // 16-bit signed integer
        int num = 0;
        int value;
        ifstream readDatFstream(filepath, ios::binary | ios::in);
        while (readDatFstream.read(
            (char*)&value, dataType // =2
        )) {
            // BSQ的存储方法
            if (num / colNum / rowNum + 1 == bandNum) {
                int row = (num / colNum) - (bandNum - 1) * rowNum; // 取得指定波段行数
                int col = num % colNum; // 获得列数
                image.at<int16_t>(row, col) = value;
            }
            num++;
        }
        return image;
    }
    else {
        cout << "falied to load an image!" << endl;
        return cv::Mat(0, 0, CV_8UC3);
        // 返回没有任何意义的空图像
    }
}