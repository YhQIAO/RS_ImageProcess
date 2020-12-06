#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include <numeric>

using namespace std;

cv::Mat getHistogramPic(cv::Mat image) {
    int nRow = image.rows;
    int nCol = image.cols;

    int his[256] = { 0 };
    for (int i = 0; i < nRow; i++) {
        for (int j = 0; j < nCol; j++) {
            int value = image.at<uchar>(i, j);
            his[value]++;
        }
    }
    int max = 0;
    for (int i = 1; i < 256; i++) {
        if (his[i] > max)
        {
            max = his[i];
        }
    }

    cv::Mat hisGraph(
        256, 256, CV_8UC1, cv::Scalar(0, 0, 0));
    for (int i = 0; i < 256; i++) {
        int height = (int)(((double)his[i] / max) * 255);
        for (int m = 255 - height + 1; m < 256; m++) {

            hisGraph.at<uchar>(m, i) = 255;
        }
    }
    return hisGraph;
}

cv::Mat HistogramNormalize(cv::Mat image) {
    int nRow = image.rows;
    int nCol = image.cols;

    int his[256] = { 0 };
    for (int i = 0; i < nRow; i++) {
        for (int j = 0; j < nCol; j++) {
            int value = image.at<uchar>(i, j);
            his[value]++;
        }
    }

    // 每个灰度值对应的概率分布
    double p[256] = { 0 };
    for (int i = 0; i < 255; i++) {
        p[i] = double(his[i]) / (nRow * nCol);
        // cout << p[i] << endl;
    }

    // 计算累计概率分布
    double s[256] = { 0 };
    for (int i = 0; i < 256; i++) {
        double sum = 0;
        for (int j = 0; j <= i; j++) {
            sum += p[j];
        }
        s[i] = sum;
    }

    // 重新映射灰度值
    int rehis[256] = { 0 };
    for (int i = 0; i < 256; i++) {
        rehis[i] = (int)(255 * s[i]);
        // 原来灰度值为i的， 灰度值变化为rehis[i]
    }

    cv::Mat reImage(image.size(), CV_8UC1);
    for (int i = 0; i < nRow; i++) {
        for (int j = 0; j < nCol; j++) {
            reImage.at<uchar>(i, j) = rehis[image.at<uchar>(i, j)];
        }
    }
    return reImage;
}

// 直方图匹配 
cv::Mat hisMatch(cv::Mat oriImage, double his[]) {
    cv::Mat ori_normalized = HistogramNormalize(oriImage);

    double sum = 0;
    for (int i = 0; i < 256; i++) {
        sum += his[i];
    }
    for (int i = 0; i < 256; i++) {
        his[i] = his[i] / sum;
    }
    
    double s[256] = { 0 };
    for (int i = 0; i < 256; i++) {
        double sum = 0;
        for (int j = 0; j <= i; j++) {
            sum += his[j];
        }
        s[i] = sum;
    }
    
    int rehis[256] = { 0 };
    for (int i = 0; i < 256; i++) {
        rehis[i] = (int)(255 * s[i]);
        // 原来灰度值为i的， 灰度值变化为rehis[i]
    } // 这个时候得到了变化函数G 即灰度值为i的， 灰度值变化为rehis[i]

    //下面反过来求，即灰度值为rehis[i](或者距离最近的）变化为i
    int inhis[256] = { 0 };
    
    for (int i = 0; i < 256; i++) {

        int minDel = 999;
        int minIndex = -1;

        for (int j = 0; j < 256; j++) {
            int del = abs(i - rehis[j]);
            if (del < minDel) {
                minDel = del;
                minIndex = j;
            }
        }
        inhis[i] = minIndex;
    }
    cv::Mat reImage(oriImage.size(), CV_8UC1);
    for (int i = 0; i < oriImage.rows; i++) {
        for (int j = 0; j < oriImage.cols; j++) {
            reImage.at<uchar>(i, j) = inhis[ori_normalized.at<uchar>(i, j)];
        }
    }
    return reImage;
}

void showArrHis(double arr[]) {
    
    double max = *max_element(arr, arr + 255);
    cv::Mat hisGraph(
        256, 256, CV_8UC1, cv::Scalar(0, 0, 0));
    for (int i = 0; i < 256; i++) {
        int height = (int)( arr[i] / max * 255*0.9);
        for (int m = 255 - height + 1; m < 256; m++) {
            hisGraph.at<uchar>(m, i) = 255;
        }
    }
    cv::imshow("his", hisGraph);
    cv::waitKey(0);
}

// 直方图匹配 
cv::Mat hisMatch(cv::Mat oriImage, cv::Mat aimImage) {
    /*
    * 先把原图像均衡化一便
    * 把标准图像也均衡化一便，记住变换的函数G(z)
    * 把原图像均衡化后的结果，用G^(-1)反过来变化一下；
    */

    int row1 = oriImage.rows;
    int col1 = oriImage.cols;

    int row2 = aimImage.rows;
    int col2 = aimImage.cols;

    // 先均衡化原图像
    cv::Mat ori_normalized = HistogramNormalize(oriImage);


    // 计算目标图像均衡化的函数
    int his[256] = { 0 };
    for (int i = 0; i < row2; i++) {
        for (int j = 0; j < col2; j++) {
            int value = aimImage.at<uchar>(i, j);
            his[value]++;
        }
    }
    double p[256] = { 0 };
    for (int i = 0; i < 255; i++) {
        p[i] = double(his[i]) / (row2 * col2);
    }
    double s[256] = { 0 };
    for (int i = 0; i < 256; i++) {
        double sum = 0;
        for (int j = 0; j <= i; j++) {
            sum += p[j];
        }
        s[i] = sum;
    }
    int rehis[256] = { 0 };
    for (int i = 0; i < 256; i++) {
        rehis[i] = (int)(255 * s[i]);
        // 原来灰度值为i的， 灰度值变化为rehis[i]
    } // 这个时候得到了变化函数G 即灰度值为i的， 灰度值变化为rehis[i]

    //下面反过来求，即灰度值为rehis[i](或者距离最近的）变化为i
    int inhis[256] = { 0 };

    for (int i = 0; i < 256; i++) {

        int minDel = 999;
        int minIndex = -1;

        for (int j = 0; j < 256; j++) {
            int del = abs(i - rehis[j]);
            if (del < minDel) {
                minDel = del;
                minIndex = j;
            }
        }
        inhis[i] = minIndex;
    }
    cv::Mat reImage(ori_normalized.size(), CV_8UC1);
    for (int i = 0; i < row1; i++) {
        for (int j = 0; j < col1; j++) {
            reImage.at<uchar>(i, j) = inhis[ori_normalized.at<uchar>(i, j)];
        }
    }
    return reImage;
}
