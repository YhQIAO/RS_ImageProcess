/*
1.自己改文件路径
2.MYfuncs头文件里写了一个函数，作用是分割字符串
    比如 "AB CD EF"  按空格分割为ab，cd和ef，然后存在一个vector里
    分割字符串也可以用上机课上讲的什么strtok，不过那个看着比较烦
3.之后是把算的NDVI图像输出成envi格式，包含一个头文件和二进制数据文件，放在一个文件夹下拿ENVI就能打开
*/


#include<iostream>
#include<opencv2/opencv.hpp>
#include"myFuncs.h"
using namespace std;
using namespace cv;  


int band1[512][512]; // 蓝色波段
int band2[512][512]; // 绿色波段
int band3[512][512]; // 红色波段
int band4[512][512]; // 近红外波段
 

int main() {

    string filepath = "..\\..\\第2章_数字图像存储\\data\\ascii_image.txt";
    fstream infile;
    infile.open(filepath);
    string s;
    for (int i = 0; i < 6; i++) {
        getline(infile, s);
        //cout << s << endl;
    }

    // read band1
    for (int i = 0; i < 512; i++) {
        getline(infile, s);
        vector<string> splited;
        splited = split_str(s, "  "); // 获取一行像素值的vector
        for (int j = 0; j < 512; j++) {
            int value = stoi(splited.at(j + 1));
            band1[i][j] = value;
        }
    }
    getline(infile, s);

    // read band2
    for (int i = 0; i < 512; i++) {
        getline(infile, s);
        vector<string> splited;
        splited = split_str(s, "  "); // 获取一行像素值的vector
        for (int j = 0; j < 512; j++) {
            int value = stoi(splited.at(j + 1));
            band2[i][j] = value;
        }
    }

    getline(infile, s);

    //read band3
    for (int i = 0; i < 512; i++) {
        getline(infile, s);
        vector<string> splited;
        splited = split_str(s, "  "); // 获取一行像素值的vector
        for (int j = 0; j < 512; j++) {
            int value = stoi(splited.at(j + 1));
            band3[i][j] = value;
        }
    }

    getline(infile, s);

    //read band4
    for (int i = 0; i < 512; i++) {
        getline(infile, s);
        vector<string> splited;
        splited = split_str(s, "  "); // 获取一行像素值的vector
        for (int j = 0; j < 512; j++) {
            int value = stoi(splited.at(j + 1));
            band4[i][j] = value;
        }
    }


    Mat RsImage(512, 512, CV_8UC3);
    for (int i = 0; i < 512; i++) {
        for (int j = 0; j < 512; j++) {
            RsImage.at<Vec3b>(i, j)[0] = band1[i][j] * 8; // 5bit -> 8Bit
            RsImage.at<Vec3b>(i, j)[1] = band2[i][j] * 8;
            RsImage.at<Vec3b>(i, j)[2] = band3[i][j] * 8;

        }
    }
    imshow("image", RsImage);
    waitKey(0);


    // 计算NDVI值
    Mat NDVI(512, 512, CV_8UC1);

    for (int i = 0; i < 512; i++) {
        for (int j = 0; j < 512; j++) {
            double NIR = (double)band4[i][j];
            double R = (double)band3[i][j];

            if (NIR < R)
            {
                NDVI.at<uchar>(i, j) = 0;
            }
            else
            {
                NDVI.at<uchar>(i, j) = 255 * ((NIR - R) / (NIR + R));
            }
        }
    }

    imshow("NDVI", NDVI);
    waitKey(0);

    // 将NDVI图像导出为ENVI标准格式
    /*
    * 一部分是HDR头文件
    * 另一部分是二进制文件
    */

    // 头文件的输出
    string hdrFilepath = "..\\..\\第2章_数字图像存储\\Code\\NDVI_output\\NDVI_image.hdr";
    string binaryFilepath = "..\\..\\第2章_数字图像存储第2章_数字图像存储\\Code\\NDVI_output\\NDVI_image.dat";

    ofstream hdrFile;
    hdrFile.open(hdrFilepath);
    hdrFile <<

        "ENVI" << endl <<
        "samples = 512" << endl <<
        "lines = 512" << endl <<
        "bands = 1" << endl <<
        "data type = 4" << endl <<
        "interleave = bsq" << endl <<
        "file type = ENVI Standard" << endl <<
        "header offset = 0" << endl <<
        "byte order = 0" << endl <<
        "band names = { NDVI band }";

    // 二进制文件的输出
    ofstream binaryio(binaryFilepath, ios::binary);

    float value;

    for (int i = 0;i < 512;i++){
        for (int j = 0; j < 512; j++)
        {
            value = NDVI.at<uchar>(i,j);
            binaryio.write(reinterpret_cast<char*>(&value), sizeof(value));
        }
    }

    binaryio.close();
}