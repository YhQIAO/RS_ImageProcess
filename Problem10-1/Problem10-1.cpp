//
// Created by 乔亦弘 on 2020/12/30.
//

#include<opencv2/opencv.hpp>
#include<iostream>
#include<fstream>
#include<vector>
#include<Eigen/Core>
#include<Eigen/Dense>

typedef cv::Vec<float, 7> Vec7f;
typedef cv::Vec<int16_t,7> Vec7i_16;

using namespace std;

// 用于存放像素点的坐标
struct Pixel{
    int x;int y;
};

Vec7f convertToDoubleVector(Vec7i_16 vi);
cv::Mat readImage(string filepath, int rowNum, int colNum);
float getDis(Vec7f v1, Vec7i_16 v2);
float Veclength(Vec7f v);
cv::Mat KNN_Classification(cv::Mat oriImage);

int main() {
    string filepath = "/Users/qiaoyihong/Library/Mobile Documents/com~apple~CloudDocs/MyUniversity/遥感数字图像处理/RS_ImageProcess/Chap11/data/Landsat8_OLI_multi_classify.dat";
    cv::Mat multiBandImage = readImage(filepath, 411,400);

    cv::Mat coloredImage(multiBandImage.size(),CV_32FC3);
    for(int i = 0;i < multiBandImage.rows;i++) {
        for(int j = 0;j < multiBandImage.cols;j++) {
            coloredImage.at<cv::Vec3f>(i, j)[0] = (float)(multiBandImage.at<cv::Vec<int16_t,7>>(i,j)[1]/ 5000.0);
            coloredImage.at<cv::Vec3f>(i, j)[1] = (float)(multiBandImage.at<cv::Vec<int16_t,7>>(i,j)[2]/ 5000.0);
            coloredImage.at<cv::Vec3f>(i, j)[2] = (float)(multiBandImage.at<cv::Vec<int16_t,7>>(i,j)[3]/ 5000.0);
        }
    }
    cv::imshow("colorImage",coloredImage);
    cv::waitKey(0);

    cv::Mat result = KNN_Classification(multiBandImage);
    cv::imshow("result", result);
    cv::waitKey(0);
}

cv::Mat readImage(string filepath, int rowNum, int colNum){
    cv::Mat band(rowNum, colNum,CV_16SC(7));
    int num = 0;
    int value;
    ifstream readDataFstream(filepath, ios::binary | ios::in);
    while (readDataFstream.read(
            (char*)&value, 2
    )) {
        int bandNum = ((num / colNum) % 7);
        int row = num / (colNum * 7);
        int col = num % colNum;
        band.at<cv::Vec<int16_t,7>>(row,col)[bandNum] = value;
        num++;
    }
    return band;
}


cv::Mat KNN_Classification(cv::Mat oriImage) {

    int Count = 0;
    cv::Mat classifiedImage(oriImage.size(), CV_8UC3);
    vector<vector<Pixel>> classes(3);
    // K分为K类
    // 看图像大概分为三类：水体植被建筑
    // 初始化特征向量
    Vec7f c1Vec(100,100,100,100,100,100,100);
    Vec7f c2Vec(200,200,200,200,200,200,200);
    Vec7f c3Vec(300,300,300,300,300,300,300);

    Vec7f deltaV1(100,100,100,100,100,100,100);
    Vec7f deltaV2(100,100,100,100,100,100,100);
    Vec7f deltaV3(100,100,100,100,100,100,100);

    do{

        Count++;
        // 清空之前的分类结果
        classes[0].clear();
        classes[1].clear();
        classes[2].clear();

        // 每一个像元，找出与之最近的聚类中心，归到这一类中去
        for (int i = 0; i < oriImage.rows; i++) {
            for (int j = 0; j < oriImage.cols; j++) {
                int index = 0;

                float d1 = getDis(c1Vec, oriImage.at<Vec7i_16>(i, j));
                float d2 = getDis(c2Vec, oriImage.at<Vec7i_16>(i, j));
                float d3 = getDis(c3Vec, oriImage.at<Vec7i_16>(i, j));
                if(d1 < d2 && d1 < d3){
                    index = 0;
                }
                if(d2 < d1 && d2 < d3){
                    index = 1;
                }
                if(d3 < d2 && d3 < d1){
                    index = 2;
                }
                Pixel p{i,j};
                classes[index].push_back(p);
            }
        }
        Vec7f aveV1(0, 0, 0, 0, 0, 0, 0);
        // 在当前的所有聚类中计算新的聚类中心
        for (int i = 0; i < classes[0].size(); i++) {
            aveV1 = aveV1 + convertToDoubleVector(oriImage.at<Vec7i_16>(classes[0].at(i).x, classes[0].at(i).y));
        }
        int num = classes[0].size();
        aveV1 = aveV1 / num;
        deltaV1 = aveV1 - c1Vec;
        c1Vec = aveV1;

        Vec7f aveV2(0, 0, 0, 0, 0, 0, 0);
        // 在当前的所有聚类中计算新的聚类中心
        for (int i = 0; i < classes[1].size(); i++) {
            aveV2 = aveV2 + convertToDoubleVector(oriImage.at<Vec7i_16>(classes[1].at(i).x, classes[1].at(i).y));
        }
        num = classes[1].size();
        aveV2 = aveV2 / num;
        deltaV2 = aveV2 - c2Vec;
        c2Vec = aveV2;

        Vec7f aveV3(0, 0, 0, 0, 0, 0, 0);
        // 在当前的所有聚类中计算新的聚类中心
        for (int i = 0; i < classes[2].size(); i++) {
            aveV3 = aveV3 + convertToDoubleVector(oriImage.at<Vec7i_16>(classes[2].at(i).x, classes[2].at(i).y));
        }
        num = classes[2].size();
        aveV3 = aveV3 / num;
        deltaV3 = aveV3 - c3Vec;
        c3Vec = aveV3;

    } while(Count < 10);

    // 显示分类之后的图片
    // 第一类
    for(int i = 0;i < classes.size();i++){
        for(int j = 0;j < classes[i].size();j++)
        {
            if(i == 0){

                classifiedImage.at<cv::Vec3b>(classes[i][j].x,
                classes[i][j].y)[0] = 0;
                classifiedImage.at<cv::Vec3b>(classes[i][j].x,
                                              classes[i][j].y)[1] = 255;
                classifiedImage.at<cv::Vec3b>(classes[i][j].x,
                                              classes[i][j].y)[2] = 0;
            }
            if(i == 1){
                classifiedImage.at<cv::Vec3b>(classes[i][j].x,
                                              classes[i][j].y)[0] = 255;
                classifiedImage.at<cv::Vec3b>(classes[i][j].x,
                                              classes[i][j].y)[1] = 0;
                classifiedImage.at<cv::Vec3b>(classes[i][j].x,
                                              classes[i][j].y)[2] = 0;
            }
            if(i == 2){
                classifiedImage.at<cv::Vec3b>(classes[i][j].x,
                                              classes[i][j].y)[0] = 0;
                classifiedImage.at<cv::Vec3b>(classes[i][j].x,
                                              classes[i][j].y)[1] = 0;
                classifiedImage.at<cv::Vec3b>(classes[i][j].x,
                                              classes[i][j].y)[2] = 255;
            }
        }
    }
    return classifiedImage;
}

float getDis(Vec7f v1, Vec7i_16 v2) {
    float squareSum = 0;
    for(int i = 0;i < 7;i++){
        squareSum+= pow(v1[i]-v2[i],2);
    }
    return sqrt(squareSum);
}

float Veclength(Vec7f v){
    float squareSum = 0;
    for(int i = 0;i < 7;i++){
        squareSum+= pow(v[i],2);
    }
    return sqrt(squareSum);
}

Vec7f convertToDoubleVector(Vec7i_16 vi){
    Vec7f vf;
    for(int i = 0;i < 7;i++){
        vf[i] = (float)vi[i];
    }
    return vf;
}