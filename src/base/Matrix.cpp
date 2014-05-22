#include "Matrix.h"
#include <stdio.h>
#include <iostream>

using namespace std;

namespace PS{
namespace MATH{



void MtxPrint(const mat44& a)
{
    for(int iRow=0; iRow<4; iRow++)
    {
        for(int iCol=0; iCol<4; iCol++)
        {
            //Print iRow
            printf("%.2f, ", a.e[iCol][iRow]);
        }
        printf("\n");
    }
}

void MtxPrint(const mat44d& a){
    for(int iRow=0; iRow<4; iRow++)
    {
        for(int iCol=0; iCol<4; iCol++)
        {
            //Print iRow
            printf("%.2f, ", a.e[iCol][iRow]);
        }
        printf("\n");    }

}




}
}
