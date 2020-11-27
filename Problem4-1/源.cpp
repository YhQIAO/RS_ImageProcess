#include<iostream>
#include"MyPCA.h"
#include"MyTCT.h"

using namespace std;

int main() {
	string filepath = "..\\..\\第4章_变换域处理方法\\data\\Landsat8_OLI_multi.dat";

	// 实现pca
	// extract_PCA(filepath, 394, 390, 7);

	// 实现缨帽变换
	TCTTransformation(filepath, 394, 390, 7);
}