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


// PCA 图像融合
// 输入参数：低分辨率的多光谱图像 高分辨率的单波段图像
void PCA_Fusion(vector<cv::Mat> MultiSpectralImage, cv::Mat highImage) {

    // 低分辩率图像放大到和高分辨率图像一样的大小
    for (int i = 0; i < MultiSpectralImage.size(); i++) {
        cv::resize(MultiSpectralImage.at(i), MultiSpectralImage.at(i), highImage.size());
    }

    int rowNum = MultiSpectralImage.at(0).rows;
    int colNum = MultiSpectralImage.at(0).cols;
    int BandNum = MultiSpectralImage.size();
    /*---------用一个vector存储每一个波段的Eigen Matrix数据---------*/
    vector<Matrix<int, Dynamic, Dynamic>> MultiBandImage; // 存储多波段影像

    for (int i = 0; i < BandNum; i++) {
        cv::Mat bandData = MultiSpectralImage.at(i);
        Matrix<int, Dynamic, Dynamic> bandMat;
        cv2eigen(bandData, bandMat);
        MultiBandImage.push_back(bandMat);
    }

    Matrix<int, Dynamic, Dynamic> highData;
    cv2eigen(highImage, highData);  // highData 存储高分辨率影像的矩阵数据

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

    Matrix<float, Dynamic, Dynamic> highVec(allPixelNum, 1);
    for (int j = 0; j < colNum; j++) {
        for (int k = 0; k < rowNum; k++) {
            highVec(j * rowNum + k, 0) = highData(k, j);
        }
    }

    /*---------归一化，求协方差矩阵--------*/
    Matrix<float, Dynamic, Dynamic> normal_DataMat(allPixelNum, BandNum);
    Matrix<float, Dynamic, Dynamic> normal_highVec(allPixelNum, 1);
    float ave1;
    for (int i = 0; i < BandNum; i++) {
        // 计算均值
        float ave = dataMat.col(i).sum() / dataMat.rows();
        if (i == 0) { ave1 = ave; }
        for (int j = 0; j < dataMat.rows(); j++) {
            normal_DataMat(j, i) = (dataMat(j, i) - ave);
        }
    }
    // 把高分辨率向量也归一化
    float high_ave = highVec.col(0).sum() / highVec.rows();
    float scale = ave1 / high_ave;
    // 归一化
    for (int j = 0; j < normal_highVec.rows(); j++) {
        normal_highVec(j, 0) = (dataMat(j, 0) - high_ave)*scale;
    }

  
    Matrix<float, Dynamic, Dynamic> normal_DataMat_T(BandNum, allPixelNum);
    normal_DataMat_T = normal_DataMat.transpose();
    Matrix<float, 7, 7> covMat = normal_DataMat_T * normal_DataMat / (allPixelNum);

    /*-------计算特征向量-------*/
    EigenSolver<Matrix<float, 7, 7>> solver(covMat);

    /*---------计算变换之后的图像---------*/
    Matrix<float, 7, 7> vectorMat = solver.pseudoEigenvectors();
    Matrix<float, 7, 7> featureValue = solver.pseudoEigenvalueMatrix();
    Matrix<float, Dynamic, Dynamic> pcaData(allPixelNum, BandNum);

    // 多光谱pca变换后的数据
    pcaData = normal_DataMat * vectorMat;
    Matrix<float, 7, 7> vectorMatInverse = vectorMat.inverse();

    // 把第一主成分给换掉
    pcaData.col(0) = normal_highVec*1.5;
    // 做个逆变换
    pcaData = pcaData * vectorMatInverse;
    
    cv::Mat result[7];
    cv::Mat colorImage = cv::Mat(highImage.size(), CV_8UC3);
    for (int i = 0; i < 7; i++) {
        Map<Matrix<float, Dynamic, Dynamic>> pca1(pcaData.col(i).data(), rowNum, colNum);
        Matrix<float, Dynamic, Dynamic>::Index a, b;
        cv::Mat temp(highImage.size(), CV_8UC1);
        double min = pca1.minCoeff(&a, &b);
        double max = pca1.maxCoeff(&a, &b);
        for (int m = 0; m < rowNum; m++) {
            for (int n = 0; n < colNum; n++) {
                temp.at<uchar>(m, n) =
                    (uchar)((pca1(m, n) - min) / (max - min) * 255);
            }
        }
        result[i] = temp;
    }
    cv::merge(result + 1, 3, colorImage);
    cv::imshow("result", colorImage);
    cv::waitKey(0);
}
