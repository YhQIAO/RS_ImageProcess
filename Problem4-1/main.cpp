#include<iostream>
#include"MyPCA.h"
#include"MyTCT.h"

using namespace std;

int main() {
	string filepath = "..\\..\\��4��_�任������\\data\\Landsat8_OLI_multi.dat";

	// ʵ��pca
	// extract_PCA(filepath, 394, 390, 7);

	// ʵ��ӧñ�任
	TCTTransformation(filepath, 394, 390, 7);
}