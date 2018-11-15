
#include "stdafx.h"
#include<stdio.h>
#include<stdlib.h>
#include <intrin.h>
#include <iostream>
#include <algorithm>
#include <memory>
#include "thread"
#include <vector>
#include <windows.h>

using namespace std;


BYTE * resultBitmap; //It is here for tests

int detectNumberOfCores() {
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	return sysinfo.dwNumberOfProcessors;
}
int getWidthOfBitmap(char* filename) 
{
	FILE* file = fopen(filename, "rb");
	unsigned char info[54];
	fread(info, sizeof(unsigned char), 54, file);

	return *(int*)&info[18];

	fclose(file);
}
int getHeightOfBitmap(char* filename)
{
	FILE* file = fopen(filename, "rb");
	unsigned char info[54];
	fread(info, sizeof(unsigned char), 54, file);

	return *(int*)&info[22];

	fclose(file);
}
int getSizeOfBmp(char *filename) 
{
	return getHeightOfBitmap(filename) * getWidthOfBitmap(filename) * 3;
}
void executeBitmapFushion(BYTE* firstBitmap, BYTE* secondBitmap,int currentIndexOfWrittingToResultBitmap,int bitmapSize)
{
	for (int i = 0; i < bitmapSize; i+=3)
	{
		//TODO: THINK ABOUT i+=3

		resultBitmap[currentIndexOfWrittingToResultBitmap +i] = ((int)firstBitmap[i] + (int)secondBitmap[i]) / 2;
		resultBitmap[currentIndexOfWrittingToResultBitmap +i + 1] = ((int)firstBitmap[i + 1] + (int)secondBitmap[i + 1]) / 2;
		resultBitmap[currentIndexOfWrittingToResultBitmap +i + 2] = ((int)firstBitmap[i + 2] + (int)secondBitmap[i + 2]) / 2;
	}
}
vector<thread> createThreads(BYTE* firstBitmap,BYTE* secondBitmap,int numberOfThreads,int bitmapSize) 
{
	vector<thread> result;
	resultBitmap = new BYTE[bitmapSize];//TODO: IT shoudn't be here
	int currentIndexOfWrittingToResultBitmap = 0;
	if (bitmapSize%numberOfThreads==0) 
	{
		int partialDataSize = bitmapSize / numberOfThreads;
		for (int i = 0; i < numberOfThreads; i++)
		{
			thread thread(executeBitmapFushion, firstBitmap, secondBitmap,currentIndexOfWrittingToResultBitmap, partialDataSize);
			result.push_back(move(thread));
			firstBitmap += partialDataSize;
			secondBitmap += partialDataSize;
			currentIndexOfWrittingToResultBitmap += partialDataSize;
		}
	}
	else 
	{
		//TODO: PADING
	}
	
	return result;
}


BYTE *LoadBitmapFile(char *filename)
{
	FILE *filePtr; //file pointer
	BITMAPFILEHEADER bitmapFileHeader; //bitmap file header
	byte *bitmapImage;  //image data array

					   //open filename in read binary mode
	filePtr = fopen(filename, "rb");

	//read the bitmap file header
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);

	//move file point to the begging of bitmap data
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	//allocate enough memory for the bitmap image data

	int bitmapSize = getSizeOfBmp(filename);

	bitmapImage = new BYTE[bitmapSize];

	//read in the bitmap image data
	fread(bitmapImage, bitmapSize, 1, filePtr);

	//close file and return bitmap iamge data
	fclose(filePtr);
	return bitmapImage;
}
void SaveBitmapToFile(BYTE* pBitmapBits,
	LONG lWidth,
	LONG lHeight,
	WORD wBitsPerPixel,
	const unsigned long& padding_size,
	LPCTSTR lpszFileName)
{
	// Some basic bitmap parameters  
	unsigned long headers_size = sizeof(BITMAPFILEHEADER) +
		sizeof(BITMAPINFOHEADER);

	unsigned long pixel_data_size = getSizeOfBmp("t5.bmp");

	BITMAPINFOHEADER bmpInfoHeader = { 0 };

	// Set the size  
	bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);

	// Bit count  
	bmpInfoHeader.biBitCount = wBitsPerPixel;

	// Set the height in pixels  
	bmpInfoHeader.biHeight = lHeight;

	// Width of the Image in pixels  
	bmpInfoHeader.biWidth = lWidth;


	// Calculate the image size in bytes  
	bmpInfoHeader.biSizeImage = pixel_data_size;

	BITMAPFILEHEADER bfh = { 0 };

	// This value should be values of BM letters i.e 0x4D42  
	// 0x4D = M 0×42 = B storing in reverse order to match with endian  
	bfh.bfType = 0x4D42;
	//bfh.bfType = 'B'+('M' << 8); 

	// <<8 used to shift ‘M’ to end  */  

	// Offset to the RGBQUAD  
	bfh.bfOffBits = headers_size;

	// Total size of image including size of headers  
	bfh.bfSize = headers_size + pixel_data_size;

	// Create the file in disk to write  
	HANDLE hFile = CreateFile(lpszFileName,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	// Return if error opening file  
	if (!hFile) return;

	DWORD dwWritten = 0;

	// Write the File header  
	WriteFile(hFile,
		&bfh,
		sizeof(bfh),
		&dwWritten,
		NULL);

	// Write the bitmap info header  
	WriteFile(hFile,
		&bmpInfoHeader,
		sizeof(bmpInfoHeader),
		&dwWritten,
		NULL);

	// Write the RGB Data  
	WriteFile(hFile,
		pBitmapBits,
		bmpInfoHeader.biSizeImage,
		&dwWritten,
		NULL);

	// Close the file handle  
	CloseHandle(hFile);
}

int main()
{
	BITMAPINFOHEADER header;
	BITMAPFILEHEADER fileHeader;
	char *bmpName1 = "t5.bmp";
	char *bmpName2 = "t6.bmp";
	int numberOfThreads = 4;
	cout << "Liczba rdzeni: " << detectNumberOfCores() << endl;
	/*auto bmp1 = readBMP(bmpName1);
	auto bmp2 = readBMP(bmpName2);*/

	auto bmp1 = LoadBitmapFile(bmpName1);
	auto bmp2 = LoadBitmapFile(bmpName2); 
	auto bmp1Size = getSizeOfBmp(bmpName1);
	auto bmp2Size = getSizeOfBmp(bmpName2);
	SaveBitmapToFile(bmp1, getWidthOfBitmap(bmpName1), getHeightOfBitmap(bmpName1), 24, 0, L"bmp1");
	SaveBitmapToFile(bmp2, getWidthOfBitmap(bmpName2), getHeightOfBitmap(bmpName2), 24, 0, L"bmp2");


	if (bmp1Size!=bmp2Size) 
	{
		cout << "Pictures have different sizes" << endl;
		return 0;
	}

	auto bmpsSize = bmp1Size;//bmp1Size same as bmp2Size
	auto threads = createThreads(bmp1, bmp2, numberOfThreads,bmpsSize);
	for (int i = 0; i < numberOfThreads; i++) 
	{
		threads[i].join();
	}
	string str = "result";
	LPCTSTR result = L"result.bmp";
	//LPCWSTR result1 = "result";
	//result1 = str.c_str();
	//drawbmp("result", getWidthOfBmp(bmpName1), getHeightOfBmp(bmpName2), bmpsSize);
	SaveBitmapToFile(resultBitmap, getWidthOfBitmap(bmpName1), getHeightOfBitmap(bmpName1), 24, 0, result);
	cout << "done" << endl;
	getchar();
	return 0;
}

