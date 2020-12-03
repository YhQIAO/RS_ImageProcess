#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <Eigen/Dense>
#include <Eigen/Core>

#define PI 3.1415926

using namespace std;
using namespace Eigen;

// 读取图像并且自动拉伸到0-255范围返回8bit位图像
cv::Mat readBinaryImageOfBandX(
    string filepath, int rowNum, int colNum,
    int bandNum, int AllBandNum, int dataType, string format) {

    if (dataType == 2) {
        cv::Mat image(rowNum, colNum, CV_16SC1);
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

            else if (format == "BIL") {
                if (((num / colNum) % AllBandNum) + 1 == bandNum) {
                    int row = num / (colNum * AllBandNum);
                    int col = num % colNum;
                    image.at<int16_t>(row, col) = value;
                    // cout << row << " " << col <<" " << image.at<int16_t>(row,col) << endl;
                }
                num++;
            }
        }
        double min, max;
        cv::minMaxLoc(image, &min, &max);
        image = (image - min) * (255.0 / (max - min));
        image.convertTo(image, CV_8UC1, 1.0);
        return image;
    }


    else if (dataType == 12) {
        cv::Mat image(rowNum, colNum, CV_16UC1);
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
        double min, max;
        cv::minMaxLoc(image, &min, &max);
        image = (image - min) * (255.0 / (max - min));
        image.convertTo(image, CV_8UC1, 1.0);
        return image;
    }


    else {
        cout << "falied to load an image!" << endl;
        return cv::Mat(0, 0, CV_8UC3);
    }
}


// 读取多光谱图像返回一个vector
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


// 8bit图像%2去极拉伸
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

cv::Mat BGR2HSI(cv::Mat oriImage) {

    // 三通道浮点数存放H， S， I
    cv::Mat hsiImage(oriImage.size(), CV_32FC3);

    int nl = oriImage.rows;
    int nc = oriImage.cols;
    for (int i = 0; i < nl; i++) {
        uchar* ori = oriImage.ptr<uchar>(i);
        float* hsi = hsiImage.ptr<float>(i);
        for (int j = 0; j < nc; j++) {
            float b = ori[j * 3] / 255.0;
            float g = ori[j * 3 + 1] / 255.0;
            float r = ori[j * 3 + 2] / 255.0;
            float num = (float)(0.5 * ((r - g) + (r - b)));
            float den = (float)sqrt((r - g) * (r - g) + (r - b) * (g - b));
            float H, S, I; // 注意全都归一化成了0-1
            if (den == 0) {    //分母不能为0
                H = 0;
            }
            else {
                double theta = acos(num / den);
                if (b <= g)
                    H = theta / (PI * 2);
                else
                    H = (2 * PI - theta) / (2 * PI);
            }
            double minRGB = min(min(r, g), b);
            den = r + g + b;
            if (den == 0)    //分母不能为0
                S = 0;
            else
                S = 1 - 3 * minRGB / den;
            I = den / 3.0;
            hsi[3 * j] = H;
            hsi[3 * j+1] = S;
            hsi[3 * j+2] = I;
        }
    }
    return hsiImage;
}

// CV_32FC3 hsi --> CV_8UC3 BGR
cv::Mat HSI2BGR(cv::Mat hsiImage) {
    cv::Mat result(hsiImage.size(), CV_8UC3);
    // hsi image is a CV_32FC3
    int row = hsiImage.rows;
    int col = hsiImage.cols;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            float preh = hsiImage.at<cv::Vec3f>(i, j)[0] * 2 * CV_PI;//H
            float pres = hsiImage.at<cv::Vec3f>(i, j)[1];  //S
            float prei = hsiImage.at<cv::Vec3f>(i, j)[2];  //I
            float r = 0, g = 0, b = 0;
            float t1, t2, t3;
            t1 = (1.0 - pres) / 3.0;
            if (preh >= 0 && preh < (CV_PI * 2 / 3)) {
                b = t1;
                t2 = pres * cos(preh);
                t3 = cos(CV_PI / 3 - preh);
                r = (1 + t2 / t3) / 3;
                r = 3 * prei * r;
                b = 3 * prei * b;
                g = 3 * prei - (r + b);
            }
            else if (preh >= (CV_PI * 2 / 3) && preh < (CV_PI * 4 / 3)) {
                r = t1;
                t2 = pres * cos(preh - 2 * CV_PI / 3);
                t3 = cos(CV_PI - preh);
                g = (1 + t2 / t3) / 3;
                r = 3 * prei * r;
                g = 3 * g * prei;
                b = 3 * prei - (r + g);
            }
            else if (preh >= (CV_PI * 4 / 3) && preh <= (CV_PI * 2)) {
                g = t1;
                t2 = pres * cos(preh - 4 * CV_PI / 3);
                t3 = cos(CV_PI * 5 / 3 - preh);
                b = (1 + t2 / t3) / 3;
                g = 3 * g * prei;
                b = 3 * prei * b;
                r = 3 * prei - (g + b);
            }
            result.at<cv::Vec3b> (i, j)[0] = int(b * 255);
            result.at<cv::Vec3b>(i, j)[1] = int(g * 255);         
            result.at<cv::Vec3b>(i, j)[2] = int(r * 255);
        }
    }
    return result;

}
