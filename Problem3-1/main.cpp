//
// Created by ����� on 2020/9/27.
//
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main() {
    
    string filepath = "..\\..\\��3��_������\\data\\Landsat8_OLI_multi.dat";
    /*
     * ͼ�������Ϣ��
     * 397�У�395�У��߸�����
     * BIL�洢��ʽ �ֽ�ƫ����0
     * ���ݸ�ʽ��16bit �з������� (ֵ���Ŵ���һ��)
     * С��
     */
    int data[397][395];
    int minValue = 999999;
    int maxValue = -1000000;

    ifstream readDatFstream(filepath, ios::binary | ios::in);
    int16_t value;
    int num = 0;
    while (readDatFstream.read(
        (char*)&value, 2
    )) {
        // ��ȡ���Ĳ���
        if (num / 395 % 7 == 3) {
            // �������ڶ�ȡ���ĸ�����
            int col = num % 395;
            int row = num / 395 / 7;

            data[row][col] = (int)value;
            
            if (value > maxValue) {
                maxValue = value;
            }
            if (value < minValue) {
                minValue = value;
            }
        }
        num++;
    }

    // ͳ��ֱ��ͼ
    int length = maxValue - minValue;
    int* his = (int*)malloc(sizeof(int) * length);
    for (int i = 0; i < length; i++) {
        his[i] = 0;
    }

    for (int i = 0; i < 397; i++) {
        for (int j = 0; j < 395; j++) {
            his[data[i][j]]++;
        }
    }
    

    // ѡ��2%-98%֮�䷶Χ
    int sum, minIndex, MaxIndex;
    sum = 0; minIndex = 0; MaxIndex = 0;
    int allPixelSum = 395 * 397;
    while (minIndex < length) {
        sum += his[minIndex];
        if (sum > allPixelSum * 0.02) {
            break;
        }
        minIndex++;
    }
    while (MaxIndex < length) {
        sum += his[MaxIndex];
        if (sum > allPixelSum * 0.98) {
            break;
        }
        MaxIndex++;
    }
    cout << minIndex << " " << MaxIndex << endl;

    // ȥ������������
    Mat testImage(397, 395, CV_8UC1);
    for (int i = 0; i < 397; i++) {
        for (int j = 0; j < 395; j++) {
            if (data[i][j] < minIndex) {
                testImage.at<uchar>(i, j) = 0;
            }
            else if (data[i][j] > MaxIndex) {
                testImage.at<uchar>(i, j) = 255;
            }
            else {
                testImage.at<uchar>(i, j) = data[i][j] / (double)(4100 - 180) * 255;
            }
        }
    }


    imshow("%2ȥ��������������", testImage);
    waitKey(0);


    Mat oriImage(397, 395, CV_8UC1);
    for (int i = 0; i < 397; i++) {
        for (int j = 0; j < 395; j++) {
            oriImage.at<uchar>(i, j) = data[i][j] / (double)(4100 - 180) * 255;
        }
    }
    imshow("ԭͼ", oriImage);
    waitKey(0);

}
