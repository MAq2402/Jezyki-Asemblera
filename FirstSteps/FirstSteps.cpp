#include "stdafx.h"
#include "thread"
#include <vector>

unsigned char * resultBMP; //It is here for tests

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

	/*for (int i = 0; i < size; i += 3)
	{
		cout << "B: " << (int)data[i];
		cout << " G: " << (int)data[i + 1];
		cout << " R: " << (int)data[i + 2];
		cout << endl;
	}*/

	return data;
}
unsigned char** divideData(unsigned char* bmp, int numberOfThreads, int dataSize)
{
	unsigned char** dividedData = new unsigned char*[1];

	dividedData[0] = bmp;

	return dividedData;
}
void executeAlgorithm(unsigned char* bmp1, unsigned char* bmp2,int size)
{
	resultBMP = new unsigned char[size];
	for (int i = 0; i < size; i+=3)
	{
		/*if (i >= size) {
			break;
		}*/
		resultBMP[i] = ((int)bmp1[i] + (int)bmp2[i]) / 2;
		resultBMP[i + 1] = ((int)bmp1[i + 1] + (int)bmp2[i + 1]) / 2;
		resultBMP[i + 2] = ((int)bmp1[i + 2] + (int)bmp2[i + 2]) / 2;

		/*if ((int)bmp1[i] != (int)bmp2[i] || (int)bmp1[i+1] != (int)bmp2[i+1]|| (int)bmp1[i+2] != (int)bmp2[i+2])
		{
			cout << "RB: " << (int)resultBMP[i] << " 1B: " << int(bmp1[i]) << " 2B: " << int(bmp2[i]) << endl;
			cout << "RB: " << (int)resultBMP[i + 1] << " 1B: " << int(bmp1[i + 1]) << "2B: " << int(bmp2[i + 1]) << endl;
			cout << "RB: " << (int)resultBMP[i + 2] << " 1B: " << int(bmp1[i + 2]) << " 2B: " << int(bmp2[i + 2]) << endl;

			getchar();
		}*/
		
	}
}
vector<thread> createThreads(unsigned char* bmp1,unsigned char* bmp2,int numberOfThreads,int dataSize) 
{
	unsigned char** dividedBmp1 = divideData(bmp1, numberOfThreads,dataSize);
	unsigned char** dividedBmp2 = divideData(bmp2, numberOfThreads, dataSize);
	vector<thread> result;


	for (int i = 0; i < numberOfThreads; i++) 
	{
		thread thread(executeAlgorithm, dividedBmp1[i], dividedBmp2[i],dataSize);
		result.push_back(move(thread));	
	}
	
	//executeAlgorithm(dividedBmp1[0], dividedBmp2[0], dataSize);

	return result;
}
bool checkIfBmpsHaveSameSize(unsigned char* bmp1, unsigned char* bmp2) {
	return true;
}
void drawbmp(char * filename,int width,int height,int size) {

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

int main()
{
	char *bmpName1 = "michal.bmp";
	char *bmpName2 = "ola.bmp";
	cout << "Liczba rdzeni: " << detectNumberOfCores() << endl;
	auto bmp1 = readBMP(bmpName1);
	auto bmp2 = readBMP(bmpName2);
	auto bmp1Size = getSizeOfBmp(bmpName1);
	auto bmp2Size = getSizeOfBmp(bmpName2);



	if (bmp1Size!=bmp2Size) 
	{
		cout << "Pictures have different sizes" << endl;
		return 0;
	}

	auto bmpsSize = bmp1Size;//bmp1Size same as bmp2Size
	auto threads = createThreads(bmp1, bmp2, 1,bmpsSize);
	threads[0].join();
	drawbmp("result", getWidthOfBmp(bmpName1), getHeightOfBmp(bmpName2), bmpsSize);
	cout << "done" << endl;
	getchar();
	return 0;
}

