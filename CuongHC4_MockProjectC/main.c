/*******************************************************************************
*Include
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "app.h"

/*******************************************************************************
*API
*******************************************************************************/
int main()
{
    uint8_t filepath[100];
    
    printf("Enter filepath (example: floppy.img): ");
    gets((char*)filepath);
    /*"floppy.img"*/
    fflush(stdin);
    system("cls");
    mainAppFat12((char*)filepath);
    return 0;
    for(;;)
    {
        printf("thay diep dep zai");
    }
}
/*******************************************************************************
*End of file
*******************************************************************************/
