#pragma once

#include<iostream>
#include<fstream>
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;

//
// Created by 乔亦弘 on 2020/9/27.
//

// C++实现对该图像的各种数学形态学处理。

#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

//int getMin(int arr[], int n) {
//    int min = 999999;
//    for (int i = 0; i < n; i++) {
//
//    }
//}


Mat readBinaryImage(string filepath, int rowNum, int colNum, int bandNum, int DataType) {
    // 单波段默认输出CV_8UC1
    Mat image(rowNum, colNum, CV_8UC1);

    int num = 0;
    uchar value;
    ifstream readDatFstream(filepath, ios::binary | ios::in);
    while (readDatFstream.read(
        (char*)&value, sizeof(value)
    )) {
        int row = num / colNum;
        int col = num % colNum;
        image.at<uchar>(row, col) = value;
        num++;
    }
    return image;
}

Mat erosionGrayImage(Mat oriImage, Mat kernel) {
    int rowNum = oriImage.size[0];
    int colNum = oriImage.size[1];

    int kernelSize = kernel.size[0];

    Mat result(rowNum - kernelSize + 1, colNum - kernelSize + 1, CV_8UC1);

    for (int i = kernelSize / 2; i < rowNum - kernelSize / 2; i++) {
        for (int j = kernelSize / 2; j < colNum - kernelSize / 2; j++) {
            
            //cout << i << " " << j << endl;
            int* value = (int*)malloc(sizeof(int) * kernelSize * kernelSize);
            // 逐个计算原图像与卷积核插值，然后取最小值
            for (int m = 0; m < kernelSize; m++) {
                for (int n = 0; n < kernelSize; n++) {
                    value[m * kernelSize + n] = oriImage.at<uchar>(i - kernelSize / 2 + m, j - kernelSize / 2 + n);
                } 
            }
            result.at<uchar>(i- kernelSize / 2, j- kernelSize / 2) = *min_element(value, value + kernelSize * kernelSize);
        }
    }
    return result;

}

Mat dilateGrayImage(Mat oriImage, Mat kernel) {
    int rowNum = oriImage.size[0];
    int colNum = oriImage.size[1];

    int kernelSize = kernel.size[0];

    Mat result(rowNum - kernelSize + 1, colNum - kernelSize + 1, CV_8UC1);

    for (int i = kernelSize / 2; i < rowNum - kernelSize / 2; i++) {
        for (int j = kernelSize / 2; j < colNum - kernelSize / 2; j++) {

            //cout << i << " " << j << endl;
            int* value = (int*)malloc(sizeof(int) * kernelSize * kernelSize);
            // 逐个计算原图像与卷积核插值，然后取最小值
            for (int m = 0; m < kernelSize; m++) {
                for (int n = 0; n < kernelSize; n++) {
                    value[m * kernelSize + n] = oriImage.at<uchar>(i - kernelSize / 2 + m, j - kernelSize / 2 + n);
                }
            }
            result.at<uchar>(i - kernelSize / 2, j - kernelSize / 2) = *max_element(value, value + kernelSize * kernelSize);
        }
    }
    return result;

}

Mat OpenGrayImage(Mat oriImage, Mat kernel) {
    Mat erosionImage = erosionGrayImage(oriImage, kernel);
    Mat dilatedImage = dilateGrayImage(erosionImage, kernel);
    return dilatedImage;
}

Mat CloseGrayImage(Mat oriImage, Mat kernel) {
    Mat dilatedImage = dilateGrayImage(oriImage, kernel);
    Mat erosionImage = erosionGrayImage(dilatedImage, kernel);
    return erosionImage;
}



