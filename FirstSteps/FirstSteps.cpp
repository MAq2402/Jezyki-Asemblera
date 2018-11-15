
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


BYTE * resultBMP; //It is here for tests

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
BYTE* readBMP(char* filename) // TODO: padding
{
	FILE* file = fopen(filename, "rb");

	int size = getSizeOfBmp(filename);

	BYTE* data = new BYTE[size];

	fread(data, sizeof(unsigned char), size, file);

	fclose(file);

	//for (int i = 0; i < size; i += 3)
	//{
	//	cout << "B: " << (int)data[i];
	//	cout << " G: " << (int)data[i + 1];
	//	cout << " R: " << (int)data[i + 2];
	//	cout << endl;
	//}

	return data;
}
unsigned char** divideData(unsigned char* bmp, int numberOfThreads, int dataSize)
{
	unsigned char** dividedData = new unsigned char*[1];

	dividedData[0] = bmp;

	return dividedData;
}
void executeAlgorithm(BYTE* bmp1, BYTE* bmp2,int start,int size)
{
	
	for (int i = 0; i < size; i+=3)
	{
		/*if (i >= size) {
			break;
		}*/
		resultBMP[start +i] = ((int)bmp1[i] + (int)bmp2[i]) / 2;
		resultBMP[start +i + 1] = ((int)bmp1[i + 1] + (int)bmp2[i + 1]) / 2;
		resultBMP[start +i + 2] = ((int)bmp1[i + 2] + (int)bmp2[i + 2]) / 2;

		/**resultBMP = ((int)*bmp1 + (int)*bmp2) / 2;
		*(resultBMP+1) = ((int)*(bmp1+1) + (int)*(bmp2+1)) / 2;
		*(resultBMP+2) = ((int)*(bmp1+2) + (int)*(bmp2+2)) / 2;*/

		/**(resultBMP) = ((int)bmp1[i] + (int)bmp2[i]) / 2;
		*(resultBMP + 1) = ((int)bmp1[i + 1] + (int)bmp2[i + 1]) / 2;
		*(resultBMP + 2) = ((int)bmp1[i + 2] + (int)bmp2[i + 2]) / 2; 

		resultBMP += 3;*/
		/*bmp1 += 3;
		bmp2 += 3;*/

		

		/*if ((int)bmp1[i] != (int)bmp2[i] || (int)bmp1[i+1] != (int)bmp2[i+1]|| (int)bmp1[i+2] != (int)bmp2[i+2])
		{
			cout << "RB: " << (int)resultBMP[i] << " 1B: " << int(bmp1[i]) << " 2B: " << int(bmp2[i]) << endl;
			cout << "RB: " << (int)resultBMP[i + 1] << " 1B: " << int(bmp1[i + 1]) << "2B: " << int(bmp2[i + 1]) << endl;
			cout << "RB: " << (int)resultBMP[i + 2] << " 1B: " << int(bmp1[i + 2]) << " 2B: " << int(bmp2[i + 2]) << endl;

			getchar();
		}*/
		
	}
}
vector<thread> createThreads(BYTE* bmp1,BYTE* bmp2,int numberOfThreads,int dataSize) 
{
	vector<thread> result;
	resultBMP = new BYTE[dataSize];
	int currentIndexOfWrittingToResultBMP = 0;
	if (dataSize%numberOfThreads==0) 
	{
		int newDataSize = dataSize / numberOfThreads;
		for (int i = 0; i < numberOfThreads; i++)
		{
			thread thread(executeAlgorithm, bmp1, bmp2,currentIndexOfWrittingToResultBMP, newDataSize);
			result.push_back(move(thread));
			bmp1 += newDataSize;
			bmp2 += newDataSize;
			currentIndexOfWrittingToResultBMP += newDataSize;
		}
	}
	else 
	{

	}
	
	return result;
}

void drawbmp(char * filename,int width,int height,int size) //TODO FIX
{

	unsigned int headers[13];
	FILE * outfile;
	int extrabytes;
	int paddedsize;
	int x; int y; int n;
	int red, green, blue;

	extrabytes = 4 - ((width * 3) % 4);                 // How many bytes of padding to add to each
														// horizontal line - the size of which must
														// be a multiple of 4 bytes.
	if (extrabytes == 4)
		extrabytes = 0;

	paddedsize = ((width * 3) + extrabytes) * height;

	// Headers...
	// Note that the "BM" identifier in bytes 0 and 1 is NOT included in these "headers".

	headers[0] = paddedsize + 54;      // bfSize (whole file size)
	headers[1] = 0;                    // bfReserved (both)
	headers[2] = 54;                   // bfOffbits
	headers[3] = 40;                   // biSize
	headers[4] = width;  // biWidth
	headers[5] = height; // biHeight

						 // Would have biPlanes and biBitCount in position 6, but they're shorts.
						 // It's easier to write them out separately (see below) than pretend
						 // they're a single int, especially with endian issues...

	headers[7] = 0;                    // biCompression
	headers[8] = paddedsize;           // biSizeImage
	headers[9] = 0;                    // biXPelsPerMeter
	headers[10] = 0;                    // biYPelsPerMeter
	headers[11] = 0;                    // biClrUsed
	headers[12] = 0;                    // biClrImportant

	outfile = fopen(filename, "wb");

	//
	// Headers begin...
	// When printing ints and shorts, we write out 1 character at a time to avoid endian issues.
	//

	fprintf(outfile, "BM");

	for (n = 0; n <= 5; n++)
	{
		fprintf(outfile, "%c", headers[n] & 0x000000FF);
		fprintf(outfile, "%c", (headers[n] & 0x0000FF00) >> 8);
		fprintf(outfile, "%c", (headers[n] & 0x00FF0000) >> 16);
		fprintf(outfile, "%c", (headers[n] & (unsigned int)0xFF000000) >> 24);
	}

	// These next 4 characters are for the biPlanes and biBitCount fields.

	fprintf(outfile, "%c", 1);
	fprintf(outfile, "%c", 0);
	fprintf(outfile, "%c", 24);
	fprintf(outfile, "%c", 0);

	for (n = 7; n <= 12; n++)
	{
		fprintf(outfile, "%c", headers[n] & 0x000000FF);
		fprintf(outfile, "%c", (headers[n] & 0x0000FF00) >> 8);
		fprintf(outfile, "%c", (headers[n] & 0x00FF0000) >> 16);
		fprintf(outfile, "%c", (headers[n] & (unsigned int)0xFF000000) >> 24);
	}

	//
	// Headers done, now write the data...
	//

	//for (y = height - 1; y >= 0; y--)     // BMP image format is written from bottom to top...
	//{
	//	for (x = 0; x <= width - 1; x++)
	//	{
	//		// Also, it's written in (b,g,r) format...

	//		fprintf(outfile, "%c", blue);
	//		fprintf(outfile, "%c", green);
	//		fprintf(outfile, "%c", red);
	//	}
	//	if (extrabytes)      // See above - BMP lines must be of lengths divisible by 4.
	//	{
	//		for (n = 1; n <= extrabytes; n++)
	//		{
	//			fprintf(outfile, "%c", 0);
	//		}
	//	}
	//}

	for (int i = 0; i < size; i++) 
	{
		fprintf(outfile, "%c", resultBMP[i]);
	}
	fclose(outfile);
	return;
}

BYTE *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader, BITMAPFILEHEADER& fileHeader)
{
	FILE *filePtr; //file pointer
	BITMAPFILEHEADER bitmapFileHeader; //bitmap file header
	byte *bitmapImage;  //image data array
	int imageIdx = 0;  //image index counter


					   //open filename in read binary mode
	filePtr = fopen(filename, "rb");
	if (filePtr == NULL)
		return NULL;

	//read the bitmap file header
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);

	//verify that this is a bmp file by check bitmap id

	if (bitmapFileHeader.bfType != 0x4D42)
	{
		fclose(filePtr);
		return NULL;
	}

	//read the bitmap info header
	fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	//move file point to the begging of bitmap data
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	//allocate enough memory for the bitmap image data
	bitmapImage = (byte*)malloc(bitmapInfoHeader->biSizeImage);

	//verify memory allocation
	if (!bitmapImage)
	{
		free(bitmapImage);
		fclose(filePtr);
		return NULL;
	}

	//read in the bitmap image data
	fread(bitmapImage, bitmapInfoHeader->biSizeImage, 1, filePtr);

	//make sure bitmap image data was read
	if (bitmapImage == NULL)
	{
		fclose(filePtr);
		return NULL;
	}
	fileHeader = bitmapFileHeader;

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

	unsigned long pixel_data_size = lHeight * ((lWidth * (wBitsPerPixel / 8)) + padding_size);

	BITMAPINFOHEADER bmpInfoHeader = { 0 };

	// Set the size  
	bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);

	// Bit count  
	bmpInfoHeader.biBitCount = wBitsPerPixel;

	// Use all colors  
	bmpInfoHeader.biClrImportant = 0;

	// Use as many colors according to bits per pixel  
	bmpInfoHeader.biClrUsed = 0;

	// Store as un Compressed  
	bmpInfoHeader.biCompression = BI_RGB;

	// Set the height in pixels  
	bmpInfoHeader.biHeight = lHeight;

	// Width of the Image in pixels  
	bmpInfoHeader.biWidth = lWidth;

	// Default number of planes  
	bmpInfoHeader.biPlanes = 1;

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

	auto bmp1 = LoadBitmapFile(bmpName1,&header,fileHeader);
	auto bmp2 = LoadBitmapFile(bmpName2,&header,fileHeader); 
	auto bmp1Size = getSizeOfBmp(bmpName1);
	auto bmp2Size = getSizeOfBmp(bmpName2);
	SaveBitmapToFile(bmp1, getWidthOfBmp(bmpName1), getHeightOfBmp(bmpName1), 24, 0, L"bmp1");
	SaveBitmapToFile(bmp2, getWidthOfBmp(bmpName2), getHeightOfBmp(bmpName2), 24, 0, L"bmp2");


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
	SaveBitmapToFile(resultBMP, getWidthOfBmp(bmpName1), getHeightOfBmp(bmpName1), 24, 0, result);
	cout << "done" << endl;
	getchar();
	return 0;
}

