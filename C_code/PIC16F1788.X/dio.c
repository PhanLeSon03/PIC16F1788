#include "dio.h"

void DioIni(void)
{
    ANSA7 = 0; /*Digital Input*/
    ANSA5 = 0;
    TRISA7 = 1;
    TRISA5 = 1;

    TRISA6 = 1;
   
}
