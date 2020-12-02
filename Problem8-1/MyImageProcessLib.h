#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <Eigen/Dense>
#include <Eigen/Core>

using namespace std;
using namespace cv;
using namespace Eigen;

Mat readBinaryImageOfBandX(
    string filepath, int rowNum, int colNum,
    int bandNum, int AllBandNum, int dataType, string format) {



    if (dataType == 2) {
        Mat image(rowNum, colNum, CV_16SC1);
        // 16-bit signed integer
        int num = 0;
        int value;
        ifstream readDatFstream(filepath, ios::binary | ios::in);
        while (readDatFstream.read(
            (char*)&value, dataType // =2
        )) {
            
            if (format == "BSQ") {
                // BSQ
                if (num / colNum / rowNum + 1 == bandNum) {
                    int row = (num / colNum) - (bandNum - 1) * rowNum;
                    int col = num % colNum;
                    image.at<int16_t>(row, col) = value;
                    // cout << row << " " << col <<" " << image.at<int16_t>(row,col) << endl;
                }
                num++;
            }
            
            else if(format == "BIL") {
                if( ((num / colNum)% AllBandNum)+1  == bandNum) {
                    int row = num / (colNum * AllBandNum) ;
                    int col = num % colNum;
                    image.at<int16_t>(row, col) = value;
                    // cout << row << " " << col <<" " << image.at<int16_t>(row,col) << endl;
                }
                num++;
            }
        }
        return image;
    }


    else if (dataType == 12) {
        Mat image(rowNum, colNum, CV_16UC1);
        // 16-bit unsigned integer
        int num = 0;
        uint16_t value;
        ifstream readDatFstream(filepath, ios::binary | ios::in);
        while (readDatFstream.read(
            (char*)&value, 2
        )) {

            if (format == "BSQ") {
                // BSQ
                if (num / colNum / rowNum + 1 == bandNum) {
                    int row = (num / colNum) - (bandNum - 1) * rowNum;
                    int col = num % colNum;
                    image.at<uint16_t>(row, col) = value;
                    // cout << row << " " << col <<" " << image.at<int16_t>(row,col) << endl;
                }
                num++;
            }

            else if (format == "BIL") {
                if (((num / colNum) % AllBandNum) + 1 == bandNum) {
                    int row = num / (colNum * AllBandNum);
                    int col = num % colNum;
                    image.at<uint16_t>(row, col) = value;
                    // cout << row << " " << col <<" " << image.at<int16_t>(row,col) << endl;
                }
                num++;
            }
        }
        return image;
    }
    

    else {
        cout << "falied to load an image!" << endl;
        return Mat(0, 0, CV_8UC3);
    }
}

vector<cv::Mat> readMultiSpectralImage(string filepath, int rowNum, int colNum,
    int bandNum, int DataType, string format) {

    vector<cv::Mat> MultiSpectralImage;
      
    for (int i = 0; i < bandNum; i++) {
        MultiSpectralImage.push_back(readBinaryImageOfBandX(
            filepath, rowNum, colNum, i + 1, bandNum, DataType, format)
        );
    }
    return MultiSpectralImage;
}


cv::Mat DepolarizationStretch_2Percent(cv::Mat oriImage) {
    // 0-255
    int* his = (int*)malloc(sizeof(int) * 255);
    for (int i = 0; i < 255; i++) {
        his[i] = 0;
    }
    for (int i = 0; i < oriImage.rows; i++) {
        for (int j = 0; j < oriImage.cols; j++) {
            his[(oriImage.at<uchar>(i, j))]++;
        }
    }

    int allPixelNum = oriImage.rows * oriImage.cols;
    int minDN, maxDN;
    int sum = 0;
    int count = 0;
    while (sum < allPixelNum * 0.01) {
        sum += his[count++];
    }
    minDN = count;

    while (sum < allPixelNum * 0.99) {
        sum += his[count++];
    }
    maxDN = count;

    cv::Mat result(oriImage.rows, oriImage.cols, CV_8UC1);
    for (int i = 0; i < oriImage.rows; i++) {
        for (int j = 0; j < oriImage.cols; j++) {
            if (oriImage.at<uchar>(i, j) < minDN) {
                result.at<uchar>(i, j) = 0;
            }
            else if (oriImage.at<uchar>(i, j) > maxDN) {
                result.at<uchar>(i, j) = 255;
            }
            else {
                result.at<uchar>(i, j) = (oriImage.at<uchar>(i, j) - minDN) / (double)(maxDN - minDN) * 255;
            }
        }
    }
    return result;
}