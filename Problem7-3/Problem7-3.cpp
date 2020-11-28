#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>

using namespace std;

cv::Mat getImage(string filename,int rowNum, int colNum);
cv::Mat FFT_Shift(cv::Mat complexI);
void onMouse(int event, int x, int y, int flags,
             void* param);

int main() {

    // 去条带

    string filepath1 = "../Chap7/data/periodic_noise.dat";
    cv::Mat OriImage = getImage(filepath1, 300,300);
    cv::Mat OriImageToShow = cv::Mat(OriImage.size(), OriImage.type());
    cv::normalize(OriImage,OriImageToShow,0,1,cv::NORM_MINMAX);
    cv::imshow("oriImage", OriImageToShow);
    cv::waitKey(0);

    cv::Mat padded;
    // 计算最适合进行fft变换的行列数
    int m = cv::getOptimalDFTSize(OriImage.rows);
    int n = cv::getOptimalDFTSize(OriImage.cols);

    // 对图像进行填充，多余的部分用0补全
    cv::copyMakeBorder(OriImage, padded,
                       0, m-OriImage.rows,
                       0, n-OriImage.cols,
                       CV_HAL_BORDER_CONSTANT,
                       cv::Scalar::all(0));

    // 傅立叶变换之后的图像是一个复数图像
    // 一个图像作为实部，一个作为虚部
    cv::Mat planes[] = {
            cv::Mat_<float>(padded),
            cv::Mat::zeros(padded.size(),CV_32F)
    };
    cv::Mat complexI;
    // 把上面的实部和虚部组合成一个双通道图像
    cv::merge(planes, 2, complexI);
    cv::dft(complexI,complexI); // 调用fft算法

    cv::split(complexI, planes);
    //即planes[0]为实部,planes[1]为虚部
    magnitude(planes[0], planes[1], planes[0]);     //planes[0] = magnitude
    cv::Mat magI = planes[0];

    magI += cv::Scalar::all(1);
    log(magI, magI);                //转换到对数尺度(logarithmic scale)
    magI = FFT_Shift(magI);
    cv::normalize(magI,magI,0,1,cv::NORM_MINMAX);

    cv::namedWindow("magI",1);
    cv::setMouseCallback("magI", onMouse, 0);
    cv::imshow("magI",magI);
    cv::waitKey(0);

    complexI = FFT_Shift(complexI);
    // 获取频谱图中亮点位置的坐标, 并去除
    // (145,158),(119,146),(157,146),(183,158)
    for(int i = 0;i < complexI.rows;i++) {
        for(int j = 0;j < complexI.cols;j++) {

            double d1 = sqrt(
                    (j-157)*(j-157)+(i-146)*(i-146));
            double d2 = sqrt(
                    (j-145)*(j-145)+(i-158)*(i-158));
            double d3 = sqrt(
                    (j-120)*(j-120)+(i-146)*(i-146));
            double d4 = sqrt(
                    (j-183)*(j-183)+(i-158)*(i-158));
            if(d1 < 5 || d2 < 5 || d3 < 5 || d4 < 5) {
                complexI.at<cv::Vec2f>(i,j)[0] = 0;
                complexI.at<cv::Vec2f>(i,j)[1] = 0;
            }
        }
    }

    complexI = FFT_Shift(complexI);
    cv::Mat ifft;
    cv::idft(complexI, ifft, cv::DFT_REAL_OUTPUT);
    cv::normalize(ifft,ifft,0,1,cv::NORM_MINMAX);

    cv::imshow("ifft",ifft);
    cv::waitKey(0);

}



cv::Mat getImage(string filename,int rowNum, int colNum) {
    cv::Mat image(rowNum, colNum, CV_32F);

    ifstream infile(filename, ios::in | ios::binary);
    float data;
    int count = 0;
    while(infile.read( (char*)(&data), 4 ) ) {
        int row = count / colNum;
        int col = count % rowNum;
        count ++;
        image.at<float>(row,col) = data;
    }
    return image;
}

// 左下右上，左上右下互换
cv::Mat FFT_Shift(cv::Mat complexI) {
    cv::Mat ReComplexI = complexI(cv::Rect(0, 0, complexI.cols&-2, complexI.rows&-2));
    int cx = ReComplexI.cols / 2;
    int cy = ReComplexI.rows / 2;
    cv::Mat q0(ReComplexI, cv::Rect(0, 0, cx, cy));       //左上角图像划定ROI区域
    cv::Mat q1(ReComplexI, cv::Rect(cx, 0, cx, cy));      //右上角图像
    cv::Mat q2(ReComplexI, cv::Rect(0, cy, cx, cy));      //左下角图像
    cv::Mat q3(ReComplexI, cv::Rect(cx, cy, cx, cy));     //右下角图像

    //变换左上角和右下角象限
    cv::Mat tmp;
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    //变换右上角和左下角象限
    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);
    return ReComplexI;
}

void onMouse(int event, int x, int y, int flags,
             void* param) {
    if(event == cv::EVENT_LBUTTONDOWN) {
        cout << "x: " << x << " y: " << y << endl;
    }
}