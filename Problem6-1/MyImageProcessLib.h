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

    if (dataType == 1) {
        cv::Mat image(rowNum, colNum, CV_8UC1);
        // 16-bit signed integer
        int num = 0;
        int value;
        ifstream readDatFstream(filepath, ios::binary | ios::in);
        while (readDatFstream.read(
                (char*)&value, dataType
        )) {
            // BSQ的存储方法
            if (num / colNum / rowNum + 1 == bandNum) {
                int row = (num / colNum) - (bandNum - 1) * rowNum; // 取得指定波段行数
                int col = num % colNum; // 获得列数
                image.at<uchar>(row, col) = value;
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

cv::Mat DepolarizationStretch_2Percent(cv::Mat oriImage) {
    // 适用于0-255
    int his[255] = {0};
    for(int i = 0;i < oriImage.rows;i++) {
        for(int j=0;j < oriImage.cols;j++) {
            his[(oriImage.at<uchar>(i,j))]++;
        }
    }

    int allPixelNum = oriImage.rows * oriImage.cols;
    int minDN, maxDN;
    int sum = 0;
    int count = 0;
    while(sum < allPixelNum * 0.01){
        sum+=his[count++];
    }
    minDN = count;

    while(sum < allPixelNum*0.99) {
        sum+=his[count++];
    }
    maxDN = count;

    cv::Mat result(oriImage.rows, oriImage.cols, CV_8UC1);
    for (int i = 0; i < oriImage.rows; i++) {
        for (int j = 0; j < oriImage.cols; j++) {
            if (oriImage.at<uchar>(i,j) < minDN) {
                result.at<uchar>(i, j) = 0;
            }
            else if (oriImage.at<uchar>(i,j) > maxDN) {
                result.at<uchar>(i, j) = 255;
            }
            else {
                result.at<uchar>(i, j) = (oriImage.at<uchar>(i,j)-minDN) / (double)(maxDN-minDN) * 255;
            }
        }
    }
    return result;
}