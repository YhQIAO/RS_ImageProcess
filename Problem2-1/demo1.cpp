/*
1.�Լ����ļ�·��
2.MYfuncsͷ�ļ���д��һ�������������Ƿָ��ַ���
    ���� "AB CD EF"  ���ո�ָ�Ϊab��cd��ef��Ȼ�����һ��vector��
    �ָ��ַ���Ҳ�������ϻ����Ͻ���ʲôstrtok�������Ǹ����űȽϷ�
3.֮���ǰ����NDVIͼ�������envi��ʽ������һ��ͷ�ļ��Ͷ����������ļ�������һ���ļ�������ENVI���ܴ�
*/


#include<iostream>
#include<opencv2/opencv.hpp>
#include"myFuncs.h"
using namespace std;
using namespace cv;  


int band1[512][512]; // ��ɫ����
int band2[512][512]; // ��ɫ����
int band3[512][512]; // ��ɫ����
int band4[512][512]; // �����Ⲩ��
 

int main() {

    string filepath = "..\\..\\��2��_����ͼ��洢\\data\\ascii_image.txt";
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
        splited = split_str(s, "  "); // ��ȡһ������ֵ��vector
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
        splited = split_str(s, "  "); // ��ȡһ������ֵ��vector
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
        splited = split_str(s, "  "); // ��ȡһ������ֵ��vector
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
        splited = split_str(s, "  "); // ��ȡһ������ֵ��vector
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


    // ����NDVIֵ
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

    // ��NDVIͼ�񵼳�ΪENVI��׼��ʽ
    /*
    * һ������HDRͷ�ļ�
    * ��һ�����Ƕ������ļ�
    */

    // ͷ�ļ������
    string hdrFilepath = "..\\..\\��2��_����ͼ��洢\\Code\\NDVI_output\\NDVI_image.hdr";
    string binaryFilepath = "..\\..\\��2��_����ͼ��洢��2��_����ͼ��洢\\Code\\NDVI_output\\NDVI_image.dat";

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

    // �������ļ������
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