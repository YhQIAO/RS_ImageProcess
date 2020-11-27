#pragma once
#include <iostream>
#include <complex>
#include <opencv2/opencv.hpp>
#include <Eigen/Dense>
#include <Eigen/Core>
#include <opencv2/core/eigen.hpp>            
#define PI 3.1415936
using namespace std;

cv::Mat BGR2CMYK(cv::Mat rgbImage) {

    cv::Mat cmyk = cv::Mat::zeros(rgbImage.rows, rgbImage.cols, CV_8UC4);
    int pixel_num = rgbImage.rows * rgbImage.cols;
    for (int i = 0; i < pixel_num; i++)
    {
        uchar c = 255 - rgbImage.data[3 * i + 2];
        uchar m = 255 - rgbImage.data[3 * i + 1];
        uchar y = 255 - rgbImage.data[3 * i + 0];
        uchar K = min(min(c, m), y);


        cmyk.data[4 * i + 0] = c;
        cmyk.data[4 * i + 1] = m;
        cmyk.data[4 * i + 2] = y;
        cmyk.data[4 * i + 3] = K;
    }
    return cmyk;
}


int rgb2hsi(cv::Mat& image, cv::Mat& hsi)
{

    int nl = image.rows;
    int nc = image.cols;
    for (int i = 0; i < nl; i++) {
        uchar* src = image.ptr<uchar>(i);
        uchar* dst = hsi.ptr<uchar>(i);
        for (int j = 0; j < nc; j++) {
            float b = src[j * 3] / 255.0;
            float g = src[j * 3 + 1] / 255.0;
            float r = src[j * 3 + 2] / 255.0;
            float num = (float)(0.5 * ((r - g) + (r - b)));
            float den = (float)sqrt((r - g) * (r - g) + (r - b) * (g - b));
            float H, S, V;
            if (den == 0) {    //分母不能为0
                H = 0;
            }
            else {
                double theta = acos(num / den);
                if (b <= g)
                    H = theta / (PI * 2);
                else
                    H = (2 * PI - theta) / (2 * PI);
            }
            double minRGB = min(min(r, g), b);
            den = r + g + b;
            if (den == 0)    //分母不能为0
                S = 0;
            else
                S = 1 - 3 * minRGB / den;
            V = den / 3.0;
            //将S分量和H分量都扩充到[0,255]区间以便于显示;
            //一般H分量在[0,2pi]之间，S在[0,1]之间
            dst[3 * j] = H * 255;
            dst[3 * j + 1] = S * 255;
            dst[3 * j + 2] = V * 255;
        }
    }
    return 0;
}