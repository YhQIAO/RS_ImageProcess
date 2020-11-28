//
// Created by 乔亦弘 on 2020/11/28.
//

// 利用提供的遥感影像base.dat和warp.dat，
// 编程实现自动特征点检测、自动同名点匹配，找出若干合格的同名点，
// 最后实现对warp.dat图像的几何校正。（可使用tie_points.pts文件内的三个同名点做为辅助）

#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>
#include "MyImageProcessLib.h"
#include <Eigen/Core>

using namespace std;

void ratioTest(vector<vector<cv::DMatch>> &matches, vector<cv::DMatch> &goodMatches);
void symmetryTest(const vector<cv::DMatch>& matches1, const vector<cv::DMatch>& matches2, vector<cv::DMatch>& symMatches);

int main() {

    string baseImagePath = "../Chap6/data/base.dat";
    string warpImagePath = "../Chap6/data/warp.dat";

    // 取BGR三个波段合成的彩色波段进行特征点的提取与匹配
    cv::Mat baseMultiBands[] = {
            DepolarizationStretch_2Percent(readBinaryImageOfBandX(baseImagePath, 2000,2000,1,6,1)),
            DepolarizationStretch_2Percent(readBinaryImageOfBandX(baseImagePath, 2000,2000,2,6,1)),
            DepolarizationStretch_2Percent(readBinaryImageOfBandX(baseImagePath, 2000,2000,3,6,1))
    };

    cv::Mat warpMultiBands[] = {
            DepolarizationStretch_2Percent(readBinaryImageOfBandX(warpImagePath, 1000,1000,1,6,1)),
            DepolarizationStretch_2Percent(readBinaryImageOfBandX(warpImagePath, 1000,1000,2,6,1)),
            DepolarizationStretch_2Percent(readBinaryImageOfBandX(warpImagePath, 1000,1000,3,6,1))
    };


    cv::Mat baseBGRImages(2000,2000,CV_8UC3);
    cv::merge(baseMultiBands,3,baseBGRImages);

    cv::Mat warpBGRImages(1000,1000,CV_8UC3);
    cv::merge(warpMultiBands,3,warpBGRImages);

    cv::imshow("baseBGRImages", baseBGRImages);
    cv::imshow("warpBGRImages", warpBGRImages);


    /*---------下面进入特征点提取的部分-----------*/
    cv::Ptr<cv::SiftFeatureDetector> ptrSIFT =
            cv::SiftFeatureDetector::create(250); // 创建一个SIFT检测器

    vector<cv::KeyPoint> baseKeyPoints, warpKeyPoints; // 存放特征点
    // 提取特征点
    ptrSIFT->detect(baseBGRImages, baseKeyPoints);
    ptrSIFT->detect(warpBGRImages, warpKeyPoints);

    cv::Mat baseDescriptor, warpDescriptor;
    ptrSIFT->compute(baseBGRImages, baseKeyPoints, baseDescriptor);
    ptrSIFT->compute(warpBGRImages, warpKeyPoints, warpDescriptor);

    cv::FlannBasedMatcher matcher;
    vector<vector<cv::DMatch>> matches1, matches2;
    vector<cv::DMatch> goodMatches1, goodMatches2, matchResult;
    matcher.knnMatch(baseDescriptor, warpDescriptor,
                     matches1,2);
    matcher.knnMatch(warpDescriptor, baseDescriptor,
                     matches2,2);
    ratioTest(matches1, goodMatches1);
    ratioTest(matches2,goodMatches2);
    symmetryTest(goodMatches1, goodMatches2, matchResult);

    cv::Mat matchImage;
    cv::drawMatches(baseBGRImages, baseKeyPoints,
                    warpBGRImages, warpKeyPoints,
                    matchResult, matchImage,
                    cv::Scalar(0,0,255),
                    cv::Scalar(0,0,255));

    cout << "共探测到" << matchResult.size() << "对特征点" << endl;
    cv::imshow("matchImage", matchImage);

    vector<cv::Point2f> baseMatchKeyPoints, warpMatchKeyPoints;
    for(vector<cv::DMatch>::iterator it = matchResult.begin();
    it!=matchResult.end();++it) {
        baseMatchKeyPoints.push_back(baseKeyPoints[it->queryIdx].pt);
        warpMatchKeyPoints.push_back(warpKeyPoints[it->trainIdx].pt);
        cout << baseKeyPoints[it->queryIdx].pt << "-->"
        << warpKeyPoints[it->trainIdx].pt << endl;
    }

    /*-----采用二次多项式模型计算--------*/
    // a0-a5  b0-b5
    Eigen::Matrix<double,6,1> M_a(6,1), M_b(6,1);
    Eigen::Matrix<double,Dynamic,1> L_a(matchResult.size(),1);
    Eigen::Matrix<double,Dynamic,1> L_b(matchResult.size(),1);
    Eigen::Matrix<double, Dynamic, 6> B_a(matchResult.size(),6);
    Eigen::Matrix<double, Dynamic, 6> B_b(matchResult.size(),6);

    for(int i = 0;i < matchResult.size();i++) {
        B_a(i,0) = 1;
        B_a(i,1) = baseMatchKeyPoints[i].x;
        B_a(i,2) = baseMatchKeyPoints[i].y;
        B_a(i,3) = baseMatchKeyPoints[i].x*baseMatchKeyPoints[i].x;
        B_a(i,4) = baseMatchKeyPoints[i].x*baseMatchKeyPoints[i].y;
        B_a(i,5) = baseMatchKeyPoints[i].y*baseMatchKeyPoints[i].y;

        B_b(i,0) = 1;
        B_b(i,1) = baseMatchKeyPoints[i].x;
        B_b(i,2) = baseMatchKeyPoints[i].y;
        B_b(i,3) = baseMatchKeyPoints[i].x*baseMatchKeyPoints[i].x;
        B_b(i,4) = baseMatchKeyPoints[i].x*baseMatchKeyPoints[i].y;
        B_b(i,5) = baseMatchKeyPoints[i].y*baseMatchKeyPoints[i].y;

        L_a(i,0) = warpMatchKeyPoints[i].x;
        L_b(i,0) = warpMatchKeyPoints[i].y;
    }

    // 最小二乘 计算a0-a5, b0-b5
    M_a = (B_a.transpose()*B_a).inverse()*B_a.transpose()*L_a;
    M_b = (B_b.transpose()*B_b).inverse()*B_b.transpose()*L_b;


    // 算一个从畸变图像到标定图像的变换
    Eigen::Matrix<double,6,1> M_a_I(6,1), M_b_I(6,1);
    Eigen::Matrix<double,Dynamic,1> L_a_I(matchResult.size(),1);
    Eigen::Matrix<double,Dynamic,1> L_b_I(matchResult.size(),1);
    Eigen::Matrix<double, Dynamic, 6> B_a_I(matchResult.size(),6);
    Eigen::Matrix<double, Dynamic, 6> B_b_I(matchResult.size(),6);

    for(int i = 0;i < matchResult.size();i++) {
        B_a_I(i,0) = 1;
        B_a_I(i,1) = warpMatchKeyPoints[i].x;
        B_a_I(i,2) = warpMatchKeyPoints[i].y;
        B_a_I(i,3) = warpMatchKeyPoints[i].x*warpMatchKeyPoints[i].x;
        B_a_I(i,4) = warpMatchKeyPoints[i].x*warpMatchKeyPoints[i].y;
        B_a_I(i,5) = warpMatchKeyPoints[i].y*warpMatchKeyPoints[i].y;

        B_b_I(i,0) = 1;
        B_b_I(i,1) = warpMatchKeyPoints[i].x;
        B_b_I(i,2) = warpMatchKeyPoints[i].y;
        B_b_I(i,3) = warpMatchKeyPoints[i].x*warpMatchKeyPoints[i].x;
        B_b_I(i,4) = warpMatchKeyPoints[i].x*warpMatchKeyPoints[i].y;
        B_b_I(i,5) = warpMatchKeyPoints[i].y*warpMatchKeyPoints[i].y;

        L_a_I(i,0) = baseMatchKeyPoints[i].x;
        L_b_I(i,0) = baseMatchKeyPoints[i].y;
    }

    // 最小二乘 计算a0-a5, b0-b5
    M_a_I = (B_a_I.transpose()*B_a_I).inverse()*B_a_I.transpose()*L_a_I;
    M_b_I = (B_b_I.transpose()*B_b_I).inverse()*B_b_I.transpose()*L_b_I;

    // 确定矫正后图像的范围，计算四个角点 0,0  0,1000  1000,0  1000,1000
    double x1 = M_a_I(0)+M_a_I(1)*0+M_a_I(2)*0
                +M_a_I(3)*0*0+M_a_I(4)*0*0+M_a_I(5)*0*0;
    double x2 = M_a_I(0)+M_a_I(1)*0+M_a_I(2)*1000
                +M_a_I(3)*0*0+M_a_I(4)*0*1000+M_a_I(5)*1000*1000;
    double x3 = M_a_I(0)+M_a_I(1)*1000+M_a_I(2)*0
                +M_a_I(3)*1000*1000+M_a_I(4)*1000*0+M_a_I(5)*0*0;
    double x4 = M_a_I(0)+M_a_I(1)*1000+M_a_I(2)*1000
                +M_a_I(3)*1000*1000+M_a_I(4)*1000*1000+M_a_I(5)*1000*1000;

    double y1 = M_b_I(0)+M_b_I(1)*0+M_b_I(2)*0
                +M_b_I(3)*0*0+M_b_I(4)*0*0+M_b_I(5)*0*0;
    double y2 = M_b_I(0)+M_b_I(1)*0+M_b_I(2)*1000
                +M_b_I(3)*0*0+M_b_I(4)*0*1000+M_b_I(5)*1000*1000;
    double y3 = M_b_I(0)+M_b_I(1)*1000+M_b_I(2)*0
                +M_b_I(3)*1000*1000+M_b_I(4)*1000*0+M_b_I(5)*0*0;
    double y4 = M_b_I(0)+M_b_I(1)*1000+M_b_I(2)*1000
                +M_b_I(3)*1000*1000+M_b_I(4)*1000*1000+M_b_I(5)*1000*1000;

    double xMIN = min(min(x1,x2),min(x3,x4));
    double xMAX = max(max(x1,x2),max(x3,x4));
    double yMIN = min(min(y1,y2),min(y3,y4));
    double yMAX = max(max(y1,y2),max(y3,y4));

    cout << xMIN << "," << yMIN << "  " << xMAX << "," << yMAX << endl;


    // 间接法，双线性插值计算矫正后的图像
    // 创建一副空的矫正后的图像
    int rows = ceil(yMAX)-floor(yMIN);
    int cols = ceil(xMAX)-floor(xMIN);

    cv::Mat rectified_band1(rows, cols,CV_8UC1);
    cv::Mat rectified_band2(rows, cols,CV_8UC1);
    cv::Mat rectified_band3(rows, cols,CV_8UC1);

    for(int i = 0;i < rectified_band1.rows;i++) {
        for(int j = 0;j < rectified_band1.cols; j++) {
            // 注意对应的像素坐标系为(j,i)
            // 矫正图中i，j处的像素对应与带矫正图像素坐标系（x，y）处
            double x_b = j+xMIN;
            double y_b = i+yMIN;

            double x = M_a(0)+M_a(1)*x_b+M_a(2)*y_b+
                    M_a(3)*x_b*x_b+M_a(4)*x_b*y_b+M_a(5)*y_b*y_b;
            double y = M_b(0)+M_b(1)*x_b+M_b(2)*y_b+
                    M_b(3)*x_b*x_b+M_b(4)*x_b*y_b+M_b(5)*y_b*y_b;

            if(x < 0 || x >= 1000 || y < 0 || y >= 1000) {
                rectified_band1.at<uchar>(i,j) = 0;
                rectified_band2.at<uchar>(i,j) = 0;
                rectified_band3.at<uchar>(i,j) = 0;
            } else {

                // 偷个懒，先用最邻近插值
                x = (int)x;
                y = (int)y;
                rectified_band1.at<uchar>(i,j) = warpBGRImages.at<cv::Vec3b>(y,x)[0];
                rectified_band2.at<uchar>(i,j) = warpBGRImages.at<cv::Vec3b>(y,x)[1];
                rectified_band3.at<uchar>(i,j) = warpBGRImages.at<cv::Vec3b>(y,x)[2];
            }
        }
    }

    cv::Mat rectified_color[] = {
            rectified_band1,
            rectified_band2,
            rectified_band3,
    };

    cv::Mat rectifiedBGRImage;
    cv::merge(rectified_color, 3, rectifiedBGRImage);

    cv::imshow("rectifiedBGRImage", rectifiedBGRImage);
    cv::waitKey(0);


}

// SIFT匹配比率检测
void ratioTest(vector<vector<cv::DMatch>> &matches, vector<cv::DMatch> &goodMatches) {
    double ratio = 0.8;
    vector<vector<cv::DMatch>> ::iterator it;
    for(it = matches.begin(); it!= matches.end();++it) {
        if( (*it)[0].distance / (*it)[1].distance < ratio ) {
            goodMatches.push_back((*it)[0]);
        }
    }
}
// 特征点匹配对称性检测
void symmetryTest(const vector<cv::DMatch>& matches1, const vector<cv::DMatch>& matches2, vector<cv::DMatch>& symMatches)
{
    symMatches.clear();
    for (vector<cv::DMatch>::const_iterator matchIterator1 = matches1.begin(); matchIterator1 != matches1.end(); ++matchIterator1)
        for (vector<cv::DMatch>::const_iterator matchIterator2 = matches2.begin(); matchIterator2 != matches2.end(); ++matchIterator2)
            if ((*matchIterator1).queryIdx == (*matchIterator2).trainIdx && (*matchIterator1).trainIdx == (*matchIterator2).queryIdx)
                symMatches.push_back(*matchIterator1);
}