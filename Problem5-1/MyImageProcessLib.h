#pragma once
//
// Created by ����� on 2020/9/29.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <Eigen/Dense>
#include <Eigen/Core>

using namespace std;
using namespace cv;
using namespace Eigen;

// ��ȡ�������ļ���ͼ�񲨶�
Mat readBinaryImageOfBandX(
    string filepath, int rowNum, int colNum,
    int bandNum, int AllBandNum, int dataType) {

    if (dataType == 2) {
        Mat image(rowNum, colNum, CV_16SC1);
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
                // cout << row << " " << col <<" " << image.at<int16_t>(row,col) << endl;
            }
            num++;
        }
        return image;
    }
    else if (dataType == 4) {
        Mat image(rowNum, colNum, CV_32FC1);
        // 16-bit signed integer
        int num = 0;
        float value;
        ifstream readDatFstream(filepath, ios::binary | ios::in);
        while (readDatFstream.read(
            (char*)&value, dataType // =2
        )) {
            // BSQ�Ĵ洢����
            if (num / colNum / rowNum + 1 == bandNum) {
                int row = (num / colNum) - (bandNum - 1) * rowNum; // ȡ��ָ����������
                int col = num % colNum; // �������
                image.at<float>(row, col) = value;
            }
            num++;
        }
        return image;
    }
    else {
        cout << "falied to load an image!" << endl;
        return Mat(0, 0, CV_8UC3);
        // ����û���κ�����Ŀ�ͼ��
    }
}

// һ���Զ�ȡ���������ֱ�Ӵ浽һ��vector��
vector<cv::Mat> readMultiSpectralImage(string filepath, int rowNum, int colNum,
    int bandNum, int DataType, string format) {

    vector<cv::Mat> MultiSpectralImage;

    if (format == "BSQ") {
        if (DataType == 4) {
            for (int i = 0; i < bandNum; i++) {
                MultiSpectralImage.push_back(readBinaryImageOfBandX(
                    filepath, rowNum, colNum, i + 1, bandNum, DataType)
                );
            }
        }
        return MultiSpectralImage;
    }
}
