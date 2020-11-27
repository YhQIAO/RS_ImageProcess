//
// Created by 乔亦弘 on 2020/11/6.
//
#include <iostream>
#include <fstream>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <Eigen/Core>
#include <Eigen/Dense>
#include "MyImageProcessLib.h"

#define PI 3.14159265359

using namespace std;

// 转弧度
double convert2Rad(double du);
// 显示多光谱图像
void showColoredImage(vector<cv::Mat> MultiSpecImage, int index);
// 计算坡向
float getAngle(float fn, double fs);
// 实现地形矫正
vector<cv::Mat> dixingjiaozheng(vector<cv::Mat> oriImage,
    double theta,
    cv::Mat& cosi, double c);


int main() {

    string DEMFilepath = "..\\..\\第5章_辐射处理\\data\\Landsat5_sd_dem.dat";
    string ImageFilepath = "..\\..\\第5章_辐射处理\\data\\Landsat5_sd_ref2.dat";
    cv::Mat DEMMat = readBinaryImageOfBandX(DEMFilepath, 500, 500, 1, 1, 2);
    DEMMat.convertTo(DEMMat, CV_32F);
    vector<cv::Mat> MultiSpecImage = readMultiSpectralImage(ImageFilepath, 500, 500, 6, 4, "BSQ");

    // 定义卷积核计算坡度坡向
    // 先计算notrh方向坡度
    cv::Mat nKernel(3, 3, CV_32F);
    nKernel.at<float>(0, 0) = 0.25; nKernel.at<float>(0, 1) = 0.5; nKernel.at<float>(0, 2) = 0.25;
    nKernel.at<float>(1, 0) = 0; nKernel.at<float>(1, 1) = 0; nKernel.at<float>(1, 2) = 0;
    nKernel.at<float>(2, 0) = -0.25; nKernel.at<float>(2, 1) = -0.5; nKernel.at<float>(2, 2) = -0.25;
    // 再计算east方向坡度
    cv::Mat eKernel(3, 3, CV_32F);
    eKernel.at<float>(0, 0) = -0.25; eKernel.at<float>(0, 1) = 0; eKernel.at<float>(0, 2) =0.25;
    eKernel.at<float>(1, 0) = -0.5; eKernel.at<float>(1, 1) = 0; eKernel.at<float>(1, 2) = 0.5;
    eKernel.at<float>(2, 0) = -0.25; eKernel.at<float>(2, 1) = 0; eKernel.at<float>(2, 2) = 0.25;

    cv::Mat nSlope(500, 500, CV_32F),
        eSlope(500, 500, CV_32F),
        SlopeMat(500, 500, CV_32F),
        AspectMat(500, 500, CV_32F);
    cv::filter2D(DEMMat, nSlope, DEMMat.depth(), nKernel);
    cv::filter2D(DEMMat, eSlope, DEMMat.depth(), eKernel);

    // 坡度 = atan(sqrt（fe^2+fn^2))
    // 坡向
    for (int i = 0; i < 500; i++) {
        for (int j = 0; j < 500; j++) {
            SlopeMat.at<float>(i, j) = atan(sqrt(
                nSlope.at<float>(i, j) * nSlope.at<float>(i, j) +
                eSlope.at<float>(i, j) * eSlope.at<float>(i, j)
            ));
            AspectMat.at<float>(i, j) = getAngle(
                nSlope.at<float>(i, j), eSlope.at<float>(i, j));
        }
    }
    // 太阳天顶角theta，太阳方位角fai，in太阳入射角
    //    SUN_AZIMUTH = 156.93529928 太阳方位角
    //    SUN_ELEVATION = 38.25092925 太阳高度角
    double fai = convert2Rad(156.93529928);
    double theta = convert2Rad(38.25092925);

    // 太阳入射角
    //cosi = cos(theta)*cos(slope)+sin(theta)*sin(slope)*cos(aspect-fai);

    // 记录每个像元值的cosi
    cv::Mat cosiMat(500-2, 500-2, CV_32F);

    for (int i = 1; i < 500-1; i++) {
        for (int j = 1; j < 500 - 1; j++) {
            cosiMat.at<float>(i - 1, j - 1) = cos(theta) * cos(SlopeMat.at<float>(i, j))
                + sin(theta) * sin(SlopeMat.at<float>(i, j))*cos(AspectMat.at<float>(i, j) - fai);
        }
    }
    // 计算C值
    int rowPixelNum = 498;
    int colPixelNum = 498;

    Eigen::Matrix<float, Dynamic, Dynamic> B_mat(498 * 498, 2);
    Eigen::Matrix<float, Dynamic, Dynamic> L_mat(498 * 498, 1);

    // 最小二乘 （B_T*B)^(-1)*B_T*L
    for (int i = 0; i < 498 * 498; i++) {

        B_mat(i, 0) = cosiMat.at<float>(i / 498, i % 498);
        B_mat(i, 1) = 1.0;

        L_mat(i, 0) = MultiSpecImage.at(4).at<float>(i / 498 + 1, i % 498 + 1);
    }

    Eigen::Matrix<float, 2, 1> kb;
    kb = (B_mat.transpose() * B_mat).inverse()* B_mat.transpose()* L_mat;
    double C = kb(1, 0) / kb(0.0);
    
    // 调用地形矫正函数
    vector<cv::Mat> result = dixingjiaozheng(MultiSpecImage, theta, cosiMat,C);
   

    showColoredImage(MultiSpecImage, 2);
    showColoredImage(result, 2);
    
}

// 转弧度
double convert2Rad(double du) {
    return du / 180 * PI;
}
// 显示多波段图片
void showColoredImage(vector<cv::Mat> MultiSpecImage,int index) {

    int row = (MultiSpecImage.at(0)).rows;
    int col = (MultiSpecImage.at(0)).cols;

    cv::Mat colorImage(row, col,CV_8UC3);

    for (int i = index; i < index+3; i++) {

        cv::Mat singleBand = MultiSpecImage.at(i);
        double minvalue, maxvalue;
        cv::minMaxIdx(singleBand, &minvalue, &maxvalue, 0, 0, noArray());
        
        singleBand /= maxvalue; // 归一化

        for (int m = 0; m < row; m ++ ) {
            for (int n = 0; n < col; n++) {
                colorImage.at<Vec3b>(m, n)[i - index] = (int)(singleBand.at<float>(m, n) * 255);
            }
        }
    }

    cv::imshow("test", colorImage);
    cv::waitKey(0);
   
}
// 计算坡向的函数
float getAngle(float fn, double fs) {
    if (fn < 0 && fs < 0){
        return atan(fs / fn);
    }
    else if (fn > 0 && fs < 0) {
        return PI - atan(-fs / fn);
    }
    else if (fn > 0 && fs > 0) {
        return PI + atan(fs / fn);
    }
    else if (fn < 0 && fs > 0) {
        return 2 * PI - atan(fs / -fn);
    }
    else if (fn == 0 && fs == 0) {
        return 0;
    }
    else if (fn == 0 && fs != 0) {
        if (fs < 0) {
            return PI / 2.0;
        }
        else {
            return 3.0 / 2 * PI;
        }
    }
    else if (fn != 0 && fs == 0) {
        if (fn > 0) {
            return PI;
        }
        else {
            return 0;
        }
    }
}

// 地形矫正
vector<cv::Mat> dixingjiaozheng(vector<cv::Mat> oriImage, 
                                double theta, 
                                cv::Mat& cosi, double c) 
{
    int row = oriImage.at(0).rows;
    int col = oriImage.at(0).cols;
    vector<cv::Mat> reImage;
    
    for (int i = 0; i < oriImage.size(); i++) {
        
        cv::Mat tempImage(row-2, col-2, CV_32F);
        
        for (int m = 0; m < row-2; m++) {
            for (int n = 0; n < col-2; n++) {
                tempImage.at<float>(m, n) = oriImage.at(i).at<float>(m + 1, n + 1) *
                    ((cos(theta) + c) / (cosi.at<float>(m, n) + c));
            }
        }
        reImage.push_back(tempImage);
    }
    return reImage;
}
