#include "stdafx.h"
#include "thread"
#include <vector>

int detectNumberOfCores() {
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	return sysinfo.dwNumberOfProcessors;
}
int getWidthOfBmp(char* filename) 
{
	FILE* file = fopen(filename, "rb");
	unsigned char info[54];
	fread(info, sizeof(unsigned char), 54, file);

	return *(int*)&info[18];

	fclose(file);
}
int getHeightOfBmp(char* filename)
{
	FILE* file = fopen(filename, "rb");
	unsigned char info[54];
	fread(info, sizeof(unsigned char), 54, file);

	return *(int*)&info[22];

	fclose(file);
}
int getSizeOfBmp(char *filename) 
{
	return getHeightOfBmp(filename) * getWidthOfBmp(filename) * 3;
}
unsigned char* readBMP(char* filename) // TODO: padding
{
	FILE* file = fopen(filename, "rb");

	int size = getSizeOfBmp(filename);

	unsigned char* data = new unsigned char[size];

	fread(data, sizeof(unsigned char), size, file);

	fclose(file);

	for (int i = 0; i < size; i += 3)
	{
		cout << "B: " << (int)data[i];
		cout << " G: " << (int)data[i + 1];
		cout << " R: " << (int)data[i + 2];
		cout << endl;
	}

	return data;
}
unsigned char** divideData(unsigned char* bmp, int numberOfThreads, int dataSize)
{
	return nullptr;
}
void executeAlgorithm(unsigned char* bmp1, unsigned char* bmp2)
{

}
vector<thread> createThreads(unsigned char* bmp1,unsigned char* bmp2,int numberOfThreads,int dataSize) 
{
	unsigned char** dividedBmp1 = divideData(bmp1, numberOfThreads,dataSize);
	unsigned char** dividedBmp2 = divideData(bmp2, numberOfThreads, dataSize);
	vector<thread> result;


	for (int i = 0; i < numberOfThreads; i++) 
	{
		thread thread(executeAlgorithm, dividedBmp1[i], dividedBmp2[i]);
		result.push_back(move(thread));	
	}

	return result;
}
bool checkIfBmpsHaveSameSize(unsigned char* bmp1, unsigned char* bmp2) {
	return true;
}
int main()
{
	cout << "Liczba rdzeni: " << detectNumberOfCores() << endl;
	auto bmp1 = readBMP("test2.bmp");
	auto bmp2 = readBMP("test2.bmp");
	auto bmp1Size = getSizeOfBmp("test2.bmp");
	auto bmp2Size = getSizeOfBmp("test2.bmp");


	if (!checkIfBmpsHaveSameSize(bmp1, bmp2)) 
	{
		cout << "Pictures have different sizes" << endl;
		return 0;
	}
	auto bmpsSize = bmp1Size;//bmp1Size same as bmp2Size
	//createThreads(bmp1, bmp2, 1,bmpsSize);
	//
	getchar();
	return 0;
}

