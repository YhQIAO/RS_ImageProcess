#pragma once
//
// Created by 乔亦弘 on 2020/9/29.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <Eigen/Dense>
#include <Eigen/Core>
#include <opencv2/core/eigen.hpp>
#include "MyImageProcessLib.h"

using namespace std;
using namespace Eigen;


void extract_PCA(
    string binaryImageFilePath,
    int rowNum,
    int colNum,
    int BandNum
) {

    // 先默认图像存储格式为BSQ（后续在拓展，先做完作业）
    // datatype = 2

    /*---------用一个vector存储每一个波段的Eigen Matrix数据---------*/
    vector<Matrix<int16_t, Dynamic, Dynamic>> MultiBandImage; // 存储多波段影像

    for (int i = 0; i < BandNum; i++) {
        cv::Mat bandData = readBinaryImageOfBandX(binaryImageFilePath, rowNum, colNum, i + 1, BandNum, 2);
        Matrix<int16_t, Dynamic, Dynamic> bandMat;

        //        imshow("band", bandData);
        //        waitKey(0);

        cv2eigen(bandData, bandMat);
        MultiBandImage.push_back(bandMat);
    }


    /*---------reshape多波段数据，每个波段为一列，行数为总像素数---------*/
    int allPixelNum = rowNum * colNum;
    // Eigen默认列优先存储
    Matrix<float, Dynamic, Dynamic> dataMat(allPixelNum, BandNum);
    for (int i = 0; i < BandNum; i++) {
        // 矩阵每一列为一个波段的数据
        for (int j = 0; j < colNum; j++) {
            for (int k = 0; k < rowNum; k++) {
                dataMat(j * rowNum + k, i) = (MultiBandImage.at(i))(k, j);
            }
        }
    }

    /*---------归一化，求协方差矩阵--------*/
    Matrix<float, Dynamic, Dynamic> aveDataMat(allPixelNum, BandNum);
    for (int i = 0; i < BandNum; i++) {
        float ave = dataMat.col(i).sum() / dataMat.rows();
        for (int j = 0; j < dataMat.rows(); j++) {
            aveDataMat(j, i) = dataMat(j, i) - ave;
        }
    }


    Matrix<float, Dynamic, Dynamic> aveDataMat_T(BandNum, allPixelNum);
    aveDataMat_T = aveDataMat.transpose();
    Matrix<float, 7, 7> covMat = aveDataMat_T * aveDataMat / allPixelNum;


    /*-------计算特征向量-------*/

    EigenSolver<Matrix<float, 7, 7>> solver(covMat);

    /*---------计算变换之后的图像---------*/
    Matrix<float, 7, 7> vectorMat = solver.pseudoEigenvectors();
    Matrix<float, 7, 7> featureValue = solver.pseudoEigenvalueMatrix();

    cout << featureValue << endl;

    cout << vectorMat << endl;

    Matrix<float, Dynamic, Dynamic> pcaData(allPixelNum, BandNum);
    pcaData = dataMat * vectorMat;

    for (int i = 0; i < 7; i++) {
        //  Matrix<float,Dynamic,Dynamic> pca1(rowNum,colNum);
        Map<Matrix<float, Dynamic, Dynamic>> pca1(pcaData.col(i).data(), rowNum, colNum);

        cv::Mat pca1Image(rowNum, colNum, CV_8UC1);

        Matrix<float, Dynamic, Dynamic>::Index a, b;
        double min = pca1.minCoeff(&a, &b);
        double max = pca1.maxCoeff(&a, &b);

        cout << "min:" << min << "  max:" << max << endl;



        for (int i = 0; i < rowNum; i++) {
            for (int j = 0; j < colNum; j++) {
                pca1Image.at<uchar>(i, j) =
                    (uchar)((pca1(i, j) - min) / (max - min) * 256);

            }
        }

        cv::imshow("image", pca1Image);
        cv::waitKey(0);
    }

}
