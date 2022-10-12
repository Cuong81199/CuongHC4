/*******************************************************************************
*Include
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "hal.h"

/*******************************************************************************
*VARIABLE
*******************************************************************************/
static FILE *sp_sloppy = NULL;
static uint16_t SectorSize = 512;

/*******************************************************************************
*API
*******************************************************************************/
uint8_t HAL_Init(const char *const FilePath)
{
    uint8_t res = 1;
    sp_sloppy = fopen(FilePath,"rb");
    if(sp_sloppy == NULL)
    {
        res = 0;
    }
    else
    {
        res = 1;
    }
    return res;
}

void HAL_update(uint16_t update)
{
    SectorSize = update;
}
int32_t HAL_ReadSector(uint32_t index,uint8_t *buff)
{
    int32_t res = SectorSize;
    int32_t check = 0;
    check = fseek(sp_sloppy,index*SectorSize,SEEK_SET);
    if (check != 0 )
    {
        res = -1;
    }
    check = 1;
    check = fread(buff,SectorSize,1,sp_sloppy);
    if (check == 0)
    {
        res = 0;
    }
    return res;
}

int32_t HAL_ReadMultiSector(uint32_t index,uint32_t num,uint8_t *buff)
{
    int32_t res = SectorSize*num;
    int32_t check = 0;
    fseek(sp_sloppy,index*SectorSize,SEEK_SET);
    if (check != 0 )
    {
        res = -1;
    }
    check = 1;
    check = fread(buff,SectorSize*num,1,sp_sloppy);
    if (check == 0)
    {
        res = 0;
    }
    return res;
}
void HAL_DeInit()
{
    fclose(sp_sloppy);
}

/*******************************************************************************
*End of file
*******************************************************************************/

