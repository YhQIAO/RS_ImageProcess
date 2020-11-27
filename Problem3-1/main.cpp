//
// Created by 乔亦弘 on 2020/9/27.
//
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main() {
    
    string filepath = "..\\..\\第3章_空域处理\\data\\Landsat8_OLI_multi.dat";
    /*
     * 图像基本信息：
     * 397行，395列，七个波段
     * BIL存储格式 字节偏移量0
     * 数据格式：16bit 有符号整数 (值被放大了一万倍)
     * 小端
     */
    int data[397][395];
    int minValue = 999999;
    int maxValue = -1000000;

    ifstream readDatFstream(filepath, ios::binary | ios::in);
    int16_t value;
    int num = 0;
    while (readDatFstream.read(
        (char*)&value, 2
    )) {
        // 读取第四波段
        if (num / 395 % 7 == 3) {
            // 代表正在读取第四个波段
            int col = num % 395;
            int row = num / 395 / 7;

            data[row][col] = (int)value;
            
            if (value > maxValue) {
                maxValue = value;
            }
            if (value < minValue) {
                minValue = value;
            }
        }
        num++;
    }

    // 统计直方图
    int length = maxValue - minValue;
    int* his = (int*)malloc(sizeof(int) * length);
    for (int i = 0; i < length; i++) {
        his[i] = 0;
    }

    for (int i = 0; i < 397; i++) {
        for (int j = 0; j < 395; j++) {
            his[data[i][j]]++;
        }
    }
    

    // 选择2%-98%之间范围
    int sum, minIndex, MaxIndex;
    sum = 0; minIndex = 0; MaxIndex = 0;
    int allPixelSum = 395 * 397;
    while (minIndex < length) {
        sum += his[minIndex];
        if (sum > allPixelSum * 0.02) {
            break;
        }
        minIndex++;
    }
    while (MaxIndex < length) {
        sum += his[MaxIndex];
        if (sum > allPixelSum * 0.98) {
            break;
        }
        MaxIndex++;
    }
    cout << minIndex << " " << MaxIndex << endl;

    // 去极化线性拉伸
    Mat testImage(397, 395, CV_8UC1);
    for (int i = 0; i < 397; i++) {
        for (int j = 0; j < 395; j++) {
            if (data[i][j] < minIndex) {
                testImage.at<uchar>(i, j) = 0;
            }
            else if (data[i][j] > MaxIndex) {
                testImage.at<uchar>(i, j) = 255;
            }
            else {
                testImage.at<uchar>(i, j) = data[i][j] / (double)(4100 - 180) * 255;
            }
        }
    }


    imshow("%2去极化线性拉伸结果", testImage);
    waitKey(0);


    Mat oriImage(397, 395, CV_8UC1);
    for (int i = 0; i < 397; i++) {
        for (int j = 0; j < 395; j++) {
            oriImage.at<uchar>(i, j) = data[i][j] / (double)(4100 - 180) * 255;
        }
    }
    imshow("原图", oriImage);
    waitKey(0);

}
