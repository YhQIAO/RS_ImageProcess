#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <numeric>
#include <algorithm>

using namespace std;

cv::Mat getImage(string filename, int rowNum, int colNum);
cv::Mat K_Neighbour_filter(cv::Mat oriImage,string type);
void AbsSort(int arr[], int n);

int main() {
    string filename = "../Chap7/data/impulse_noise.dat";
    cv::Mat image;
    image = getImage(filename, 300, 300);
    cv::imshow("original image", image);

    cv::Mat aveFilterImage = K_Neighbour_filter(image,"ave");
    cv::imshow("aveFilterImage", aveFilterImage);
    cv::Mat middleFilterImage = K_Neighbour_filter(image,"middle");
    cv::imshow("middleFilterImage", aveFilterImage);

    cv::waitKey(0);
}

cv::Mat K_Neighbour_filter(cv::Mat oriImage, string type) {
    int rowNum = oriImage.rows;
    int colNum = oriImage.cols;

    cv::Mat filtered_Image(rowNum-2, colNum-2,CV_8UC1);

    for(int i = 1; i< rowNum-1;i++) {
        for(int j = 1; j < colNum-1;j++) {

            // 获取3*3模版内的像素值与中心的差值
            int arr[9];
            arr[0] = oriImage.at<uchar>(i,j)-oriImage.at<uchar>(i-1,j-1);
            arr[1] = oriImage.at<uchar>(i,j)-oriImage.at<uchar>(i-1,j);
            arr[2] = oriImage.at<uchar>(i,j)-oriImage.at<uchar>(i-1,j+1);
            arr[3] = oriImage.at<uchar>(i,j)-oriImage.at<uchar>(i,j-1);
            arr[4] = oriImage.at<uchar>(i,j)-oriImage.at<uchar>(i,j);
            arr[5] = oriImage.at<uchar>(i,j)-oriImage.at<uchar>(i,j+1);
            arr[6] = oriImage.at<uchar>(i,j)-oriImage.at<uchar>(i+1,j-1);
            arr[7] = oriImage.at<uchar>(i,j)-oriImage.at<uchar>(i+1,j);
            arr[8] = oriImage.at<uchar>(i,j)-oriImage.at<uchar>(i+1,j+1);

            // 排序后取5个最邻近的像素值
            AbsSort(arr,9);
            int knn[5];
            for(int i =0;i< 9;i++) {
                knn[i] = arr[i];
            }
            if(type == "ave") {
                int ave = accumulate(knn,knn+5,0) / 5.0;
                filtered_Image.at<uchar>(i-1,j-1) = oriImage.at<uchar>(i,j)-ave;
            }
            if(type == "middle") {
                filtered_Image.at<uchar>(i-1,j-1) = oriImage.at<uchar>(i,j)-knn[3];
            }
        }
    }
    return filtered_Image;
}

// 读取二进制影像
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

void AbsSort(int arr[], int n){
    for(int i = 0;i < n-1;i++) {
        for(int j = 0;j < n-1-i;j++) {
            if (abs(arr[j]) > abs(arr[j+1])) {
                int temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
        }
    }
}
