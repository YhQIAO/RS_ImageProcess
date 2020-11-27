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
            // BSQ�Ĵ洢����
            if (num / colNum / rowNum + 1 == bandNum) {
                int row = (num / colNum) - (bandNum - 1) * rowNum; // ȡ��ָ����������
                int col = num % colNum; // �������
                image.at<int16_t>(row, col) = value;
            }
            num++;
        }
        return image;
    }
    else {
        cout << "falied to load an image!" << endl;
        return cv::Mat(0, 0, CV_8UC3);
        // ����û���κ�����Ŀ�ͼ��
    }
}