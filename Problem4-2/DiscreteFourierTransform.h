#pragma once
//
// Created by 乔亦弘 on 2020/10/9.
//
#define _USE_MATH_DEFINES 
#include <math.h>

#include <iostream>
#include <fstream>
#include <complex.h>
#include <Eigen/Dense>
#include <Eigen/Core>
#include <opencv2/opencv.hpp>
#include <opencv2/core/eigen.hpp>

#define PI 3.141592653

using namespace std;
using namespace Eigen;


// 实现傅里叶变换
// 先按行做一遍一维傅里叶变换，再按列做一遍

cv::Mat DFT(cv::Mat oriImage) {
	int rowNum = oriImage.rows;
	int colNum = oriImage.cols;

	Matrix<double, Dynamic, Dynamic> imageData;
	cv::cv2eigen(oriImage, imageData);

	for (int i = 0; i < rowNum; i++) {
		for (int j = 0; j < colNum; j++) {
			imageData(i, j) = imageData(i, j) * pow(-1, (i + j));
		}
	}

	Matrix<complex<double>, Dynamic, Dynamic> complexImage(rowNum, colNum);
	// 按行做傅里叶变换
	
	for (int i = 0; i < rowNum; i++) {
		cout << i << endl;
		for (int j = 0; j < colNum; j++) {
			
			complex<double> temp{ 0,0 };
			for (int x = 0; x < colNum; x++) {
				complex<double> temp1 = {
					cos(2 * PI * j * x / colNum),
					-sin(2*PI*j*x/ colNum)
				};
				temp += imageData(i, x) * temp1;
			}
			complexImage(i, j) = temp * (1.0/colNum);
		}
	}

	Matrix<complex<double>, Dynamic, Dynamic> recomplexImage = complexImage;

	for (int i = 0; i < colNum; i++) {
		cout << i << endl;
		for (int j = 0; j < rowNum; j++) {

			complex<double> temp{ 0,0 };
			for (int x = 0; x < rowNum; x++) {
				complex<double> temp1 = {
					cos(2 * PI * j * x / rowNum),
					-sin(2 * PI * j * x / rowNum)
				};
				temp += recomplexImage(x, i) * temp1;
			}
			complexImage(j,i) = temp * (1.0 / rowNum);
		}
	}

	Matrix<double, Dynamic, Dynamic> pImage(394, 390);
	cv::Mat result(rowNum, colNum, CV_8UC1);

	for (int i = 0; i < rowNum; i++) {
		for (int j = 0; j < colNum; j++) {
			pImage(i,j) = log(abs(complexImage(i, j)));
			// cout << pImage(i, j) << endl;
		}
	}

	
	double min = pImage.minCoeff();
	double max = pImage.maxCoeff();

	for (int i = 0; i < rowNum; i++) {
		for (int j = 0; j < colNum; j++) {
			result.at<uchar>(i, j) = (int)( 
				(pImage(i, j)-min)*255 / (max-min) 
				);
		}
	}
	
	return result;

}
//float getREAt_uv(cv::Mat& Image, int u, int v, int M, int N);
//cv::Mat DFT(cv::Mat image) {
//    // 傅立叶变换
//    
//    cv::Mat fouriImage(image.rows, image.cols, CV_16SC1);
//    Matrix<int16_t, Dynamic, Dynamic> llimage(image.rows, image.cols);
//
//    for (int u = 0; u < image.rows; u++) {
//        for (int v = 0; v < image.cols; v++) {
//            image.at<int16_t>(u, v) *= pow(-1, u + v);
//        }
//    }
//
//    float max = -1;
//    // 用变换公式将空间域转化至频率域
//    // f(u,v) = sigma_0_M-1(sigma_0_N-1( f(x,y)*
//    for (int u = 0; u < image.rows; u++) {
//        cout << u << endl;
//        for (int v = 0; v < image.cols; v++) {
//            float fuv = getREAt_uv(image, u, v, image.rows, image.cols);
//            fuv = log(fuv + 1);
//            llimage(u, v) = fuv;
//            if (fuv > max) {
//                max = fuv;
//            }
//        }
//    }
//
//    cv::Mat eightBitImage(image.rows, image.cols, CV_8UC1);
//
//    for (int u = 0; u < fouriImage.rows; u++) {
//        for (int v = 0; v < fouriImage.cols; v++) {
//            eightBitImage.at<uchar>(u, v) = (int)llimage(u, v) / max * 255;
//        }
//    }
//
//  
//    return eightBitImage;
//    
//}
//
//float getREAt_uv(cv::Mat& Image, int u, int v, int M, int N) {
//    float result = 0;
//    int MN = M * N;
//    float reSum = 0;
//    float imSum = 0;
//    for (int x = 0; x < M; x++) {
//        for (int y = 0; y < N; y++) {
//            // 幅角
//            float theta = -2 * M_PI * (
//                (float(u * x) / M) + (float(v * y) / N)
//                );
//            // 模
//            int length = (int)Image.at<int16_t>(x, y);
//            // 实部
//            float re = length * cos(theta);
//            // 虚部
//            float im = length * sin(theta);
//            reSum += re;
//            imSum += im;
//        }
//    }
//    result = sqrt(
//        pow(reSum, 2) + pow(imSum, 2)
//    );
//    return result / MN;
//}

