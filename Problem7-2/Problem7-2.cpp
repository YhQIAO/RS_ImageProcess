//
// Created by 乔亦弘 on 2020/11/27.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>

using namespace std;

cv::Mat getImage(string filename,int rowNum, int colNum);
cv::Mat OpenCV_FFT(cv::Mat oriImage);

int main() {
    string filepath1 = "../Chap7/data/gaussian_noise.dat";
    cv::Mat gaussian_noise_mage = getImage(filepath1, 300,300);
    cv::Mat fft_complex_image = OpenCV_FFT(gaussian_noise_mage);

    cv::Mat ifft;
    cv::idft(fft_complex_image, ifft, cv::DFT_REAL_OUTPUT);
    cv::normalize(ifft,ifft,0,1,cv::NORM_MINMAX);
    cv::imshow("ifft",ifft);
    cv::waitKey(0);

}



cv::Mat getImage(string filename,int rowNum, int colNum) {
    cv::Mat image(rowNum, colNum, CV_8UC1);

    ifstream infile(filename, ios::in | ios::binary);
    int data;
    int count = 0;
    while(infile.read( (char*)(&data), 1 ) ) {
        int row = count / colNum;
        int col = count % rowNum;
        count ++;
        image.at<uchar>(row,col) = data;
    }
    return image;
}

cv::Mat OpenCV_FFT(cv::Mat oriImage) {
    cv::Mat padded;
    int m = cv::getOptimalDFTSize(oriImage.rows);
    int n = cv::getOptimalDFTSize(oriImage.cols);

    cv::copyMakeBorder(oriImage, padded,
                       0, m-oriImage.rows,
                       0, n-oriImage.cols,
                       CV_HAL_BORDER_CONSTANT,
                       cv::Scalar::all(0));

    cv::Mat planes[] = {
            cv::Mat_<float>(padded),
            cv::Mat::zeros(padded.size(),CV_32F)
    };

    cv::Mat complexI;
    cv::merge(planes, 2, complexI);
    cv::dft(complexI,complexI);

    return complexI;

    cv::split(complexI, planes);
    cv::magnitude(planes[0], planes[1], planes[0]);
    cv::Mat magI = planes[0];
    magI += cv::Scalar::all(1);
    cv::log(magI, magI);

    magI = magI(cv::Rect(0, 0, magI.cols&-2, magI.rows&-2));

    int cx = magI.cols / 2;
    int cy = magI.rows / 2;

    cv::Mat q0(magI, cv::Rect(0, 0, cx, cy));       //左上角图像划定ROI区域
    cv::Mat q1(magI, cv::Rect(cx, 0, cx, cy));      //右上角图像
    cv::Mat q2(magI, cv::Rect(0, cy, cx, cy));      //左下角图像
    cv::Mat q3(magI, cv::Rect(cx, cy, cx, cy));     //右下角图像

    //变换左上角和右下角象限
    cv::Mat tmp;
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    //变换右上角和左下角象限
    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);

    cv::normalize(magI, magI, 0, 1,cv::NORM_MINMAX);
    return magI;
}

