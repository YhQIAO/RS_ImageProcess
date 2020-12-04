#include <iostream>
#include <opencv2/opencv.hpp>

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
            256, 256, CV_8UC1,cv::Scalar(0,0,0));
    for (int i = 0; i < 256; i++) {
        int height = (int) (((double)his[i] / max) * 255);
        for(int m = 255-height+1;m < 256;m++) {
            hisGraph.at<uchar>(m,i) = 255;
        }
    }
    return hisGraph;
}

cv::Mat HistogramNormalize(cv::Mat image) {
    int nRow = image.rows;
    int nCol = image.cols;

    //
    int his[256] = { 0 };
    for (int i = 0; i < nRow; i++) {
        for (int j = 0; j < nCol; j++) {
            int value = image.at<uchar>(i, j);
            his[value]++;
        }
    }

    // 每个灰度值对应的概率分布
    double p[256]  = {0};
    for(int i = 0;i < 255;i++) {
        p[i] = double(his[i]) / (nRow*nCol);
        // cout << p[i] << endl;
    }

    // 计算累计概率分布
    double s[256] = {0};
    for(int i = 0;i < 256;i++) {
        double sum = 0;
        for(int j = 0;j <= i;j++) {
            sum+= p[j];
        }
        s[i] = sum;
    }

    // 重新映射灰度值
    int rehis[256] = {0};
    for(int i = 0;i < 256;i++) {
        rehis[i] = (int)(255*s[i]);
        // 原来灰度值为i的， 灰度值变化为rehis[i]
    }

    cv::Mat reImage(image.size(),CV_8UC1);
    for(int i = 0;i < nRow;i++) {
        for(int j =0;j < nCol;j++) {
            reImage.at<uchar>(i,j) = rehis[image.at<uchar>(i,j)];
        }
    }
    return reImage;
}