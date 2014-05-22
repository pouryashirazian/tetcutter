/*
 * PS_DebugUtils.cpp
 *
 *  Created on: 2012-02-16
 *      Author: pourya
 */
#include "DebugUtils.h"
#include "base/FileDirectory.h"
#include <iostream>
#include <fstream>
#include <stdio.h>

using namespace std;
using namespace PS;
using namespace PS::FILESTRINGUTILS;

namespace PS{
namespace DEBUGTOOLS{

int SaveArrayCSV(const char* chrFilePath, float* lpArray, U32 count)
{
	AnsiStr strLine;
	for(U32 i=0; i<count; i++)
	{
		if(i < count - 1)
			strLine += printToAStr("%f, ", lpArray[i]);
		else
			strLine += printToAStr("%f", lpArray[i]);
	}

	WriteTextFile(AnsiStr(chrFilePath), strLine);
	return count;
}

bool SaveArray(const char* chrFilePath, double* lpArray, U32 count) {
	ofstream myfile;
	myfile.open (chrFilePath);

	char buffer[1024];
	sprintf(buffer, "%d\n", count);
	myfile << buffer;
	for(U32 i=0; i<count; i++) {
		sprintf(buffer, "%f\n", lpArray[i]);
		myfile << buffer;
	}
	myfile.close();	return count;
}

bool LoadArray(const char* chrFilePath, double** lpArray, U32& count) {

	count = 0;
	string line;
	ifstream myfile (chrFilePath);
	if (myfile.is_open())
	{
		//Read count
		if(myfile.good()) {
			getline(myfile, line);
			count = atoi(line.c_str());
			if(count > 0)
				*lpArray = new double[count];
			else
				return false;
		}

		int index = 0;
		while ( myfile.good() )
		{
			getline (myfile,line);
			*lpArray[index] = atof(line.c_str());

			if(index >= count) {
				printf("Out of bounds!\n");
				myfile.close();
				return false;
			}
		}
		myfile.close();
	}

	return (count > 0);
}


void PrintArray(const U32* lpData, U32 count)
{
	printf("\n");
	for(U32 i=0; i<count; i++)
	{
		printf("%d, ", lpData[i]);
	}
	printf("\n");
}

void PrintArrayF(const float* lpData, U32 count)
{
	for(U32 i=0; i<count; i++)
	{
		printf("%.2f, ", lpData[i]);
	}
	printf("\n");
}

void FillArray(U32* lpData, U32 count, U32 nMin, U32 nMax)
{
	for(int i=0; i < count; i++)
		lpData[i] = static_cast<U32>(RandRangeT<float>(nMin, nMax));
}

void FillArrayF(float* lpData, U32 count, float nMin, float nMax)
{
	for(int i=0; i < count; i++)
		lpData[i] = RandRangeT<float>(nMin, nMax);
}

}
}
