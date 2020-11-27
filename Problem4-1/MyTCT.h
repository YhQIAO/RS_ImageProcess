#pragma once
//
// Created by ����� on 2020/10/9.
//
/*
 * ʵ��ӧñ�任
 * �任�����Ѿ�����
 *
 *
 */

#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <Eigen/Dense>
#include <Eigen/Core>
#include <opencv2/core/eigen.hpp>
#include <Eigen/Eigenvalues>
#include "MyImageProcessLib.h"

using namespace std;
using namespace cv;
using namespace Eigen;

void TCTTransformation(
    string binaryImageFilePath,
    int rowNum,
    int colNum,
    int BandNum
) {

    // TCT coefficients for Landsat 8 at-satellite reflectance.
    Matrix<float, 6, 6> TctCoeffMat(6, 6);
    TctCoeffMat << 0.3029, 0.2786, 0.4733, 0.5599, 0.508, 0.1872,
        -0.2941, -0.243, -0.5424, 0.7276, 0.0713, -0.1608,
        0.1511, 0.1973, 0.3283, 0.3407, -0.7117, -0.4559,
        -0.8239, 0.0849, 0.4396, -0.058, 0.2013, -0.2773,
        -0.3294, 0.0557, 0.1056, 0.1855, -0.4349, 0.8085,
        0.1079, -0.9023, 0.4119, 0.0575, -0.0259, 0.0252;

    // ��ǰ��������Ϊ��Brightness��Greenness��WetNess
    // �ж�Ӧ�Ĳ���Ϊ blue 2�� green 3�� red 4�� NIR 5��SWIR1 6��SWIR 7

    // ��ȡ�����ļ���Ϊ����
    vector<Matrix<int16_t, Dynamic, Dynamic>> MultiBandImage; // �洢�ನ��Ӱ��
    for (int i = 0; i < BandNum; i++) {
        Mat bandData = readBinaryImageOfBandX(binaryImageFilePath, rowNum, colNum, i + 1, BandNum, 2);
        Matrix<int16_t, Dynamic, Dynamic> bandMat;

        cv2eigen(bandData, bandMat);
        MultiBandImage.push_back(bandMat);
    }


    // ����2-6���� ÿһ��Ϊһ�����ε�������ɵ�������
    int allPixelNum = rowNum * colNum;
    // EigenĬ�������ȴ洢
    Matrix<float, Dynamic, Dynamic> dataMat(allPixelNum, 6);
    for (int i = 0; i < BandNum - 1; i++) {  // ע���һ������û��
        // ����ÿһ��Ϊһ�����ε�����
        for (int j = 0; j < colNum; j++) {
            for (int k = 0; k < rowNum; k++) {
                dataMat(j * rowNum + k, i) = (MultiBandImage.at(i + 1))(k, j);
            }
        }
    }

    // ����任֮���ͼ��
    Matrix<float, Dynamic, Dynamic> TCTData(allPixelNum, BandNum);
    TCTData = dataMat * TctCoeffMat.transpose();

    // ��ʾͼ��
    for (int i = 0; i < 6; i++) {
        //  Matrix<float,Dynamic,Dynamic> pca1(rowNum,colNum);
        Map<Matrix<float, Dynamic, Dynamic>> tct(TCTData.col(i).data(), rowNum, colNum);

        Mat tctImage(rowNum, colNum, CV_8UC1);

        Matrix<float, Dynamic, Dynamic>::Index a, b;
        double min = tct.minCoeff(&a, &b);
        double max = tct.maxCoeff(&a, &b);

       


        for (int i = 0; i < rowNum; i++) {
            for (int j = 0; j < colNum; j++) {
              
                tctImage.at<uchar>(i, j) =
                    (uchar)((tct(i, j) - min) / (max - min) * 256);

            }
        }

        imshow("image", tctImage);
        waitKey(0);
    }





}

