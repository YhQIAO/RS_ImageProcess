#pragma once
//
// Created by 乔亦弘 on 2020/10/9.
//
/*
 * 实现缨帽变换
 * 变换矩阵已经给出
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

    // 行前三个依次为：Brightness，Greenness，WetNess
    // 列对应的波段为 blue 2， green 3， red 4， NIR 5，SWIR1 6，SWIR 7

    // 读取数据文件存为矩阵
    vector<Matrix<int16_t, Dynamic, Dynamic>> MultiBandImage; // 存储多波段影像
    for (int i = 0; i < BandNum; i++) {
        Mat bandData = readBinaryImageOfBandX(binaryImageFilePath, rowNum, colNum, i + 1, BandNum, 2);
        Matrix<int16_t, Dynamic, Dynamic> bandMat;

        cv2eigen(bandData, bandMat);
        MultiBandImage.push_back(bandMat);
    }


    // 波段2-6重塑 每一列为一个波段的像素组成的列向量
    int allPixelNum = rowNum * colNum;
    // Eigen默认列优先存储
    Matrix<float, Dynamic, Dynamic> dataMat(allPixelNum, 6);
    for (int i = 0; i < BandNum - 1; i++) {  // 注意第一个波段没用
        // 矩阵每一列为一个波段的数据
        for (int j = 0; j < colNum; j++) {
            for (int k = 0; k < rowNum; k++) {
                dataMat(j * rowNum + k, i) = (MultiBandImage.at(i + 1))(k, j);
            }
        }
    }

    // 计算变换之后的图像
    Matrix<float, Dynamic, Dynamic> TCTData(allPixelNum, BandNum);
    TCTData = dataMat * TctCoeffMat.transpose();

    // 显示图像
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

