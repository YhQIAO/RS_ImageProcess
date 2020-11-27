#pragma once
//
// Created by ����� on 2020/9/29.
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

    // ��Ĭ��ͼ��洢��ʽΪBSQ����������չ����������ҵ��
    // datatype = 2

    /*---------��һ��vector�洢ÿһ�����ε�Eigen Matrix����---------*/
    vector<Matrix<int16_t, Dynamic, Dynamic>> MultiBandImage; // �洢�ನ��Ӱ��

    for (int i = 0; i < BandNum; i++) {
        cv::Mat bandData = readBinaryImageOfBandX(binaryImageFilePath, rowNum, colNum, i + 1, BandNum, 2);
        Matrix<int16_t, Dynamic, Dynamic> bandMat;

        //        imshow("band", bandData);
        //        waitKey(0);

        cv2eigen(bandData, bandMat);
        MultiBandImage.push_back(bandMat);
    }


    /*---------reshape�ನ�����ݣ�ÿ������Ϊһ�У�����Ϊ��������---------*/
    int allPixelNum = rowNum * colNum;
    // EigenĬ�������ȴ洢
    Matrix<float, Dynamic, Dynamic> dataMat(allPixelNum, BandNum);
    for (int i = 0; i < BandNum; i++) {
        // ����ÿһ��Ϊһ�����ε�����
        for (int j = 0; j < colNum; j++) {
            for (int k = 0; k < rowNum; k++) {
                dataMat(j * rowNum + k, i) = (MultiBandImage.at(i))(k, j);
            }
        }
    }

    /*---------��һ������Э�������--------*/
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


    /*-------������������-------*/

    EigenSolver<Matrix<float, 7, 7>> solver(covMat);

    /*---------����任֮���ͼ��---------*/
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
