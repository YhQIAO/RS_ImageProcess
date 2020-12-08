#include<iostream>
#include <opencv2/opencv.hpp>

using namespace std;

cv::Mat unSharp_Mask(cv::Mat oriImage) {
    // 先模糊一下
    cv::Mat gusssinBluredImage;
    cv::GaussianBlur(oriImage, gusssinBluredImage,
                     cv::Size(7,7),7,7);
    // 相减求取边缘信息
    cv::Mat edge = oriImage-gusssinBluredImage;
    return edge;
}

cv::Mat Robert(cv::Mat oriImage) {
    cv::Mat edge(oriImage.size(),CV_8UC1,cv::Scalar(0));
    for(int i = 0;i < oriImage.rows-1;i++) {
        for(int j = 0;j < oriImage.cols-1;j++) {
            int gx,gy;
            gx = oriImage.at<uchar>(i+1,j+1)-oriImage.at<uchar>(i,j);
            gy = oriImage.at<uchar>(i+1,j)-oriImage.at<uchar>(i,j+1);
            edge.at<uchar>(i,j) = (int)(sqrt(
                    gx*gx+gy*gy));
        }
    }
    return edge;
}

cv::Mat Sobel(cv::Mat oriImage) {
    cv::Mat horizontal(3,3,CV_16SC1);
    horizontal.at<int16_t>(0,0) = -1;horizontal.at<int16_t>(0,1) = 0;horizontal.at<int16_t>(0,2) = 1;
    horizontal.at<int16_t>(1,0) = -2;horizontal.at<int16_t>(1,1) = 0;horizontal.at<int16_t>(1,2) = 2;
    horizontal.at<int16_t>(2,0) = -1;horizontal.at<int16_t>(2,1) = 0;horizontal.at<int16_t>(2,2) = 1;
    cout << horizontal << endl;
    cv::Mat vertical(3,3,CV_16SC1);
    vertical.at<int16_t>(0,0) = -1;vertical.at<int16_t>(0,1) = -2;vertical.at<int16_t>(0,2) = -1;
    vertical.at<int16_t>(1,0) = 0;vertical.at<int16_t>(1,1) = 0;vertical.at<int16_t>(1,2) = 0;
    vertical.at<int16_t>(2,0) = 1;vertical.at<int16_t>(2,1) = 2;vertical.at<int16_t>(2,2) = 1;
    cv::Mat gx,gy;
    cv::filter2D(oriImage,gx,oriImage.depth(),horizontal);
    cv::filter2D(oriImage,gy,oriImage.depth(),vertical);

    cv::Mat edge(oriImage.size(),oriImage.type());
    for(int i = 0;i < oriImage.rows;i++) {
        for(int j = 0;j < oriImage.cols;j++) {
            edge.at<uchar>(i,j) = (int)(sqrt(
                    gx.at<uchar>(i,j)*gx.at<uchar>(i,j)+
                            gy.at<uchar>(i,j)*gy.at<uchar>(i,j)));
        }
    }
    return edge;
}

