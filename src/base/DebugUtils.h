#ifndef DEBUGUTILS_H_
#define DEBUGUTILS_H_

#include "vec.h"

namespace ps {
namespace utils {

//All Debug Utility Functions
int SaveArrayCSV(const char* chrFilePath, float* lpArray, U32 count);
bool SaveArray(const char* chrFilePath, double* lpArray, U32 count);
bool LoadArray(const char* chrFilePath, double** lpArray, U32& count);


void PrintArray(const U32* lpData, U32 count);
void PrintArrayF(const float* lpData, U32 count);

void FillArray(U32* lpData, U32 count, U32 nMin = 0, U32 nMax = 100);
void FillArrayF(float* lpData, U32 count, float nMin = 0.0f, float nMax = 100.0f);

}
}

#endif
