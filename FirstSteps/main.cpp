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


const WORD bitsPerPixel = 24;
const LPCTSTR nameOfResultBitmapFile = L"result.bmp";
const WORD bfTypeOfBitmapFileHeader = 0x4D42;

int detectNumberOfCores() 
{
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
void executeBitmapFushion(BYTE* firstBitmap, BYTE* secondBitmap,BYTE* resultBitmap,int currentIndexOfWrittingToResultBitmap,int bitmapSize)
{
	for (int i = 0; i < bitmapSize; i+=3)
	{
		resultBitmap[currentIndexOfWrittingToResultBitmap + i] = ((int)firstBitmap[i] + (int)secondBitmap[i]) / 2;
		resultBitmap[currentIndexOfWrittingToResultBitmap + i + 1] = ((int)firstBitmap[i + 1] + (int)secondBitmap[i + 1]) / 2;
		resultBitmap[currentIndexOfWrittingToResultBitmap + i + 2] = ((int)firstBitmap[i + 2] + (int)secondBitmap[i + 2]) / 2;	
	}
}
vector<thread> addThreadsToVector(int vectorLength,BYTE* &firstBitmap,BYTE* &secondBitmap,BYTE* resultBitmap,int &currentIndexOfWrittingToResultBitmap,int partialDataSize)
{
	vector<thread> result;
	for (int i = 0; i < vectorLength; i++)
	{
		thread thread(executeBitmapFushion, firstBitmap, secondBitmap, resultBitmap, currentIndexOfWrittingToResultBitmap, partialDataSize);
		result.push_back(move(thread));
		firstBitmap += partialDataSize;
		secondBitmap += partialDataSize;
		currentIndexOfWrittingToResultBitmap += partialDataSize;
	}

	return result;
}
void addThreadToVector(vector<thread>& vectorOfThreads,BYTE* firstBitmap, BYTE* secondBitmap, BYTE* &resultBitmap, int &currentIndexOfWrittingToResultBitmap, int partialDataSize)
{
	thread thread(executeBitmapFushion, firstBitmap, secondBitmap, resultBitmap, currentIndexOfWrittingToResultBitmap, partialDataSize);
	vectorOfThreads.push_back(move(thread));
	firstBitmap += partialDataSize;
	secondBitmap += partialDataSize;
	currentIndexOfWrittingToResultBitmap += partialDataSize;
}
vector<thread> createThreads(BYTE* firstBitmap,BYTE* secondBitmap,BYTE* resultBitmap,int numberOfThreads,int bitmapSize) 
{
	vector<thread> result;
	int currentIndexOfWrittingToResultBitmap = 0;
	int partialDataSize = bitmapSize / numberOfThreads;
	if (bitmapSize%numberOfThreads==0) 
	{
		
		result = addThreadsToVector(numberOfThreads, firstBitmap, secondBitmap, resultBitmap, currentIndexOfWrittingToResultBitmap, partialDataSize);
	}
	else 
	{
		int sizeThatLeft = bitmapSize%numberOfThreads;
		int firstpartialDataSize = sizeThatLeft + partialDataSize;
		// first thread with additional data
	    vector<thread> subResult = addThreadsToVector(1, firstBitmap, secondBitmap, resultBitmap, currentIndexOfWrittingToResultBitmap, firstpartialDataSize);
		//currentIndexOfWrittingToResultBitmap += firstpartialDataSize;
		/*firstBitmap += firstpartialDataSize;
		secondBitmap += firstpartialDataSize;*/
		result = addThreadsToVector(numberOfThreads-1, firstBitmap, secondBitmap, resultBitmap, currentIndexOfWrittingToResultBitmap, partialDataSize);
		result.insert(result.begin(),move(subResult[0]));
		/*result = addThreadsToVector(numberOfThreads - 1, firstBitmap, secondBitmap, resultBitmap, currentIndexOfWrittingToResultBitmap, partialDataSize);
		currentIndexOfWrittingToResultBitmap += partialDataSize * 6;
		vector<thread> subResult = addThreadsToVector(1, firstBitmap, secondBitmap, resultBitmap, currentIndexOfWrittingToResultBitmap, firstpartialDataSize);
		
		
		result.push_back(move(subResult[0]));*/
	/*		for (int i = 0; i < numberOfThreads; i++)
		{
			if (i == 0)
			{
				thread thread(executeBitmapFushion, firstBitmap, secondBitmap, resultBitmap, currentIndexOfWrittingToResultBitmap, partialDataSize+sizeThatLeft);
				result.push_back(move(thread));
				firstBitmap += partialDataSize+sizeThatLeft;
				secondBitmap += partialDataSize+sizeThatLeft;
				currentIndexOfWrittingToResultBitmap += partialDataSize+sizeThatLeft;
			}
			else 
			{
				thread thread(executeBitmapFushion, firstBitmap, secondBitmap, resultBitmap, currentIndexOfWrittingToResultBitmap, partialDataSize);
				result.push_back(move(thread));
				firstBitmap += partialDataSize;
				secondBitmap += partialDataSize;
				currentIndexOfWrittingToResultBitmap += partialDataSize;
			}			
			
			
		}*/

		/*for (int i = 0; i < numberOfThreads; i++)
		{
		if (i == 0)
		{
			addThreadToVector(result, firstBitmap, secondBitmap, resultBitmap, currentIndexOfWrittingToResultBitmap, partialDataSize + sizeThatLeft);
			currentIndexOfWrittingToResultBitmap += partialDataSize + sizeThatLeft;
		}
		else
		{
		thread thread(executeBitmapFushion, firstBitmap, secondBitmap, resultBitmap, currentIndexOfWrittingToResultBitmap, partialDataSize);
		result.push_back(move(thread));
		firstBitmap += partialDataSize;
		secondBitmap += partialDataSize;
		currentIndexOfWrittingToResultBitmap += partialDataSize;
		}*/


		}
	
	return result;
}


BYTE *loadBitmapFromFile(char *filename)
{
	FILE *file;
	BITMAPFILEHEADER bitmapFileHeader;
	BYTE *result;

	file = fopen(filename, "rb");

	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, file);

	fseek(file, bitmapFileHeader.bfOffBits, SEEK_SET);

	int bitmapSize = getSizeOfBmp(filename);

	result = new BYTE[bitmapSize];

	fread(result, bitmapSize, 1, file);

	fclose(file);

	return result;
}
BITMAPINFOHEADER setUpBitmapInfoHeader(int height,int width,int bitmapSize) 
{
	BITMAPINFOHEADER bitmapInfoHeader = { 0 };

	bitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);

	bitmapInfoHeader.biBitCount = bitsPerPixel;

	bitmapInfoHeader.biHeight = height;

	bitmapInfoHeader.biWidth = width;

	bitmapInfoHeader.biSizeImage = bitmapSize;

	return bitmapInfoHeader;
}

BITMAPFILEHEADER setUpBitmapFileHeader(unsigned long sizeOfBitmapHeader,int bitmapSize) 
{
	BITMAPFILEHEADER bitmapFileHeader = { 0 };

	bitmapFileHeader.bfType = bfTypeOfBitmapFileHeader;

	bitmapFileHeader.bfOffBits = sizeOfBitmapHeader;

	bitmapFileHeader.bfSize = sizeOfBitmapHeader + bitmapSize;

	return bitmapFileHeader;
}
void saveBitmapToFile(BYTE* bitmap,int width,int height,int bitmapSize)
{ 
	unsigned long sizeOfBitmapHeader = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	BITMAPINFOHEADER bitmapInfoHeader = setUpBitmapInfoHeader(height, width, bitmapSize);

	BITMAPFILEHEADER bitmapFileHeader = setUpBitmapFileHeader(sizeOfBitmapHeader,bitmapSize);

	HANDLE resultFileHandler = CreateFile(nameOfResultBitmapFile,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

	WriteFile(resultFileHandler,&bitmapFileHeader,sizeof(bitmapFileHeader),0,NULL);

	WriteFile(resultFileHandler,&bitmapInfoHeader,sizeof(bitmapInfoHeader),0,NULL);
 
	WriteFile(resultFileHandler,bitmap,bitmapInfoHeader.biSizeImage,0,NULL);
 
	CloseHandle(resultFileHandler);
}

int main()
{
	try 
	{
		char *firstBitmapFileName = "t5.bmp";
		char *secondBitmapFileName = "t6.bmp";
		int numberOfThreads = 7;
		cout << "Liczba rdzeni: " << detectNumberOfCores() << endl;

		BYTE* firstBitmap = loadBitmapFromFile(firstBitmapFileName);
		BYTE* secondBitmap = loadBitmapFromFile(secondBitmapFileName);
		int sizeOfFirstBitmap = getSizeOfBmp(firstBitmapFileName);
		int sizeOfSecondBitmap = getSizeOfBmp(secondBitmapFileName);


		if (sizeOfFirstBitmap != sizeOfSecondBitmap)
		{
			cout << "Pictures have different sizes" << endl;
			return 0;
		}

		//it does not matter which size i take
		BYTE * resultBitmap = new BYTE[sizeOfFirstBitmap];

		// bitmap size does not matter since they are same
		vector<thread> threads = createThreads(firstBitmap, secondBitmap, resultBitmap, numberOfThreads, sizeOfFirstBitmap);
		for (int i = 0; i < numberOfThreads; i++)
		{
			threads[i].join();
		}
		// again bitmap size, height and width does not matter, since ther are same
		saveBitmapToFile(resultBitmap, getWidthOfBitmap(firstBitmapFileName), getHeightOfBitmap(firstBitmapFileName), sizeOfFirstBitmap);

		cout << "done" << endl;
		unsigned _int64 a = 210, b = 55;
		cout << MyProc1(a, b);

		getchar();

		return 0;
	}
	catch (exception e) 
	{
		cout << "Exception has been thrown: "  << +e.what() << endl;
	}
}

