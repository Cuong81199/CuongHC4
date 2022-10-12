/*******************************************************************************
*Include
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "FATFS.h"
#include "hal.h"
/*******************************************************************************
*VARIABLE
*******************************************************************************/
static FATFS_BootSector_Struct_t BootInfo;
/* Allocate a variable containing the Boot Sector read properties */
/*******************************************************************************
*STATIC
*******************************************************************************/

/**
 * @brief  convert the input data to seconds value
 */
static uint16_t read_Second(uint16_t a);

/**
 * @brief  convert the input data to minute value
 */
static uint16_t read_Minute(uint16_t a);

/**
 * @brief  convert the input data to hour value
 */
static uint16_t read_Hour(uint16_t a);

/**
 * @brief  convert the input data to day value
 */
static uint16_t read_Day(uint16_t a);

/**
 * @brief  convert the input data to month value
 */
static uint16_t read_Month(uint16_t a);

/**
 * @brief  convert the input data to year value
 */
static uint16_t read_Year(uint16_t a);

/**
 * @brief  check if emtry is empty
 *
 * @param  pointer to the first character of entry
 * @return 1 : no empty| 0: empty
 */
static uint8_t Check_BuffEntry(uint8_t *_pt);

/**
 * @brief  read data first Cluster next in fat
 *
 * @param  a: current first cluster
 *         pt: pointer to the Fat table
 * @return uint32_t : next first cluster value
 */
static uint32_t CalculFat12(uint32_t a,uint8_t *pt);

/**
 * @brief  create a node in the style of linklist
 *
 * @param  data: value data of node
 * @return pointer to that note
 */
static FATFS_Node* createNode(FATFS_Entry_Struct_t data);

/**
 * @brief  insert a node at the end of the list
 *
 * @param  *head: pointer to list
 *         value: data of insert node
 * @return pointer to list
 */
static FATFS_Node* addTail(FATFS_Node* head,FATFS_Entry_Struct_t value);

/**
 * @brief  delete the last node in the list
 *
 * @param  *head: pointer to list
 * @return pointer to list
 */
static FATFS_Node* DelHead(FATFS_Node* head);

static uint16_t read_Second(uint16_t a)
{
    return(2*(a&(0|31)));
}

static uint16_t read_Minute(uint16_t a)
{
    return((a>>5)&(0|63));
}

static uint16_t read_Hour(uint16_t a)
{
    return((a>>11)&(0|63));
}

static uint16_t read_Day(uint16_t a)
{
    return(a&(0|31));
}

static uint16_t read_Month(uint16_t a)
{
    return((a>>5)&(0|15));
}

static uint16_t read_Year(uint16_t a)
{
    return(((a>>9)&(0|127))+1980);
}

static uint8_t Check_BuffEntry(uint8_t *_pt)
{
    uint8_t res = 0;
    for(uint8_t _index = 0; _index < 32;_index++)
    {
        if(*(_pt + _index) != 0)
        {
            res = 1;
        }
    }
    return res;
}

static uint32_t CalculFat12(uint32_t a,uint8_t *pt)
{
    uint32_t temp = 0;
    uint32_t res = 0;
    temp = a * 1.5;
    if(a % 2 == 0)
    {
        res = ((*(pt+temp+1)&(15))*256) + (*(pt+temp));
    }
    else
    {
        res = ((*(pt+temp))>>4) + (*(pt+temp+1)*16);
    }
    return res;
}

static FATFS_Node* createNode(FATFS_Entry_Struct_t data)
{
    FATFS_Node* temp;
    uint8_t index = 0;
    temp = malloc(sizeof(FATFS_Node));
    temp->next = NULL;
    temp->data.EntryDay = data.EntryDay;
    temp->data.EntryFarther = data.EntryFarther;
    temp->data.EntryFirstCluster = data.EntryFirstCluster;
    temp->data.EntryHour = data.EntryHour;
    temp->data.EntryMinute = data.EntryMinute;
    temp->data.EntryMonth = data.EntryMonth;
    temp->data.EntryYear = data.EntryYear;
    for(index = 0; index < 8; index++)
    {
        temp->data.EntryName[index] = data.EntryName[index];
    }
    for(index = 0; index < 3; index++)
    {
        temp->data.EntryNameExpand[index] = data.EntryNameExpand[index];
    }
    temp->data.EntrySecond = data.EntrySecond;
    temp->data.EntrySize = data.EntrySize;
    temp->data.EntryType = data.EntryType;
    return temp;
}

static FATFS_Node* addTail(FATFS_Node* head,FATFS_Entry_Struct_t value)
{
    FATFS_Node* temp;
    FATFS_Node* p;
    temp = createNode(value);
    if(head == NULL)
    /*Linked list is empty*/
    {
        head = temp;
    }
    else
    {
        p = head;
    while(p->next != NULL)
        {
            p = p->next;
        }
        p->next = temp;
    }
    return head;
}

static FATFS_Node* DelHead(FATFS_Node* head)
{
    FATFS_Node* p;
    p = head;
    if(head == (NULL)){
    }else{
        head = head->next;
        free(p);
    }
    return head;
}

/*******************************************************************************
*API
*******************************************************************************/
uint8_t FATFS_Init(const char *FilePath,FATFS_BootSector_Struct_t *pt_boot)
{
    uint8_t res = 0;
    res = HAL_Init(FilePath);
    FATFS_ReadBoot(pt_boot);
    HAL_update(pt_boot->bytePerSector);
    BootInfo.bytePerSector = pt_boot->bytePerSector;
    BootInfo.EntryPerRoot = pt_boot->EntryPerRoot;
    BootInfo.NumberFat = pt_boot->NumberFat;
    BootInfo.SectorPerBoots = pt_boot->SectorPerBoots;
    BootInfo.SectorPerCluster = pt_boot->SectorPerCluster;
    BootInfo.SectorPerFat = pt_boot->SectorPerFat;
    return res;
}

void FATFS_DeInit()
{
    HAL_DeInit();
}

uint8_t FATFS_Access(uint32_t Position,FATFS_Node **head)
{
    uint32_t index;
    uint8_t res = 0;
    FATFS_Node *tempNode = *head;
    
    for(index = 0;index < Position;index++)
    {
        tempNode = tempNode->next;
    }
    uint32_t firstCluster = tempNode->data.EntryFirstCluster;
    uint8_t typeEntry = tempNode->data.EntryType;
    if(typeEntry == 16)
    /*this element is a folder*/
    {
        if(firstCluster == 0)
        /*this element is a root*/
        {
            res = 0;
        }
        else
        {
            res = 1;
        }
    }
    else if(typeEntry == 0)
    /*this element is a filer*/
    {
        res = 2;
    }
    return res;
}

void FATFS_ReadBoot(FATFS_BootSector_Struct_t *pt)
{
    int32_t check = 0;
    uint8_t *buff = NULL;

    buff = malloc(512);
    check = HAL_ReadSector(0,buff);
    //printf("%d",check);
    if(check != 512)
    {
        printf("False read boot");
    }
    pt->bytePerSector = buff[12]*256 + buff[11];
    pt->SectorPerCluster = buff[13];
    pt->SectorPerBoots = buff[15]*256 + buff[14];
    pt->NumberFat = buff[16];
    pt->EntryPerRoot = buff[18]*256 + buff[17];
    pt->SectorPerFat = buff[23]*256 + buff[22];
    free(buff);
}

void FATFS_ReadRoot(uint8_t pos,uint32_t _sizeRoot,FATFS_Node **head,uint32_t *countEntry)
{
    uint8_t *buff = NULL;
    /* allocates a buffer containing the data of root */
    int32_t check = 0;
    /* check in read process*/
    uint16_t index = 0;
    uint8_t *pt = NULL;
    FATFS_Entry_Struct_t data;

    buff = malloc(_sizeRoot);
    pt = buff;
    check = HAL_ReadMultiSector(pos,(BootInfo.EntryPerRoot*32/BootInfo.bytePerSector),buff);
    if(check != (int32_t)BootInfo.EntryPerRoot*32)
    /* check HAL_readMultisector*/
    {
        printf("False read Root Sector!");
    }
    /*create buffer store root sector*/
    while(*head != NULL)
    /* delete all linked list */
    {
        *head = DelHead(*head);
    }
    (*countEntry) = 0;
    /*count Entry in linked list = 0*/
    while(Check_BuffEntry(pt) == 1)
    {

        pt = &buff[index*32];
        if((*(pt + 11) == 0)&&(Check_BuffEntry(pt)==1))
        /* entry is file */
        {
            for(uint8_t index1 = 0;index1<8;index1++)
            {
                data.EntryName[index1] = *(pt + index1);
            }
            for(uint8_t index1 = 0;index1<3;index1++)
            {
                data.EntryNameExpand[index1] = *(pt + index1 + 8);
            }
            data.EntryType = *(pt+11);
            data.EntryHour = read_Hour(*(pt + 22) + *(pt + 23)*256);
            data.EntryMinute = read_Minute(*(pt + 22) + *(pt + 23)*256);
            data.EntrySecond = read_Second(*(pt + 22) + *(pt + 23)*256);
            data.EntryDay = read_Day(*(pt + 24) + *(pt + 25)*256);
            data.EntryMonth = read_Month(*(pt + 24) + *(pt + 25)*256);
            data.EntryYear = read_Year(*(pt + 24) + *(pt + 25)*256);
            data.EntryFirstCluster = ( *(pt+26)+*(pt+27)*256);
            data.EntrySize = *(pt+31)*256*256*256+*(pt+30)*256*256+*(pt+29)*256+*(pt+28);
            *head = addTail(*head,data);
            (*countEntry)++;
        }
        else if((*(pt + 11) == 16)&&(Check_BuffEntry(pt)==1))
        /*entry is folder*/
        {
            for(uint8_t index1 = 0;index1<8;index1++)
            {
                data.EntryName[index1] = *(pt + index1);
            }
            data.EntryType = *(pt+11);
            data.EntryHour = read_Hour(*(pt + 22) + *(pt + 23)*256);
            data.EntryMinute = read_Minute(*(pt + 22) + *(pt + 23)*256);
            data.EntrySecond = read_Second(*(pt + 22) + *(pt + 23)*256);
            data.EntryDay = read_Day(*(pt + 24) + *(pt + 25)*256);
            data.EntryMonth = read_Month(*(pt + 24) + *(pt + 25)*256);
            data.EntryYear = read_Year(*(pt + 24) + *(pt + 25)*256);
            data.EntryFirstCluster = ( *(pt+26)+*(pt+27)*256);
            data.EntrySize =*(pt+31)*256*256*256+*(pt+30)*256*256+*(pt+29)*256+*(pt+28);
            *head = addTail(*head,data);
            (*countEntry)++;
        }
        index++;
        for(uint8_t index = 0;index<3;index++)
        {
            data.EntryNameExpand[index] = 32;
        }
    }
    free(buff);
}

void FATFS_ReadSubDirectory(uint32_t Position,FATFS_Node **head,uint32_t *countEntry)
{
    uint8_t *buff = NULL;
    /* Allocate a buffer containing the data of a cluster */
    uint8_t *buffFAT = NULL;
    /* Allocates a buffer containing the data of fat */
    uint32_t index = 0;
    int32_t check = 0;
    uint8_t *pt = NULL;
    uint32_t firstCluster = 0;
    FATFS_Node* temp = NULL;
    temp = *head;
    FATFS_Entry_Struct_t data;

    buff = malloc(BootInfo.bytePerSector);
    buffFAT = malloc(BootInfo.bytePerSector*BootInfo.SectorPerFat);
    pt = buff;
    check = HAL_ReadMultiSector((uint32_t)BootInfo.SectorPerBoots,(uint32_t)BootInfo.SectorPerFat,buffFAT);
    /*BootInfo.SectorPerBoots: index of FAT | BootInfo.SectorPerFat: Secter of FAT*/
    if (check != BootInfo.SectorPerFat*BootInfo.bytePerSector)
    {
        printf("False read Subdirectory");
    }
    for(index = 0;index < Position;index++)
    {
        temp = temp->next;
    }
    firstCluster = temp->data.EntryFirstCluster;
    check = HAL_ReadSector((uint32_t)(firstCluster + BootInfo.SectorPerFat*BootInfo.NumberFat + BootInfo.SectorPerBoots + BootInfo.EntryPerRoot*32/BootInfo.bytePerSector - 2),buff);
    /*(firstCluster + BootInfo.SectorPerFat*BootInfo.NumberFat + BootInfo.SectorPerBoots + BootInfo.EntryPerRoot*32/BootInfo.bytePerSector - 2): index of cluster*/
    while(*head != NULL)
    /*delete all linked list*/
    {
        *head = DelHead(*head);
    }
    (*countEntry) = 0;
    index = 0;
    while(firstCluster != 4095)
    {
    while(Check_BuffEntry(pt) == 1)
    {

        pt = (buff+(index*32));
        for(uint8_t index1 = 0;index1 < 8;index1++)
        {
            data.EntryName[index1] = 32;
        }
        for(uint8_t index1 = 0;index1 < 3;index1++)
        {
            data.EntryNameExpand[index1] = 32;
        }
        if((*(pt + 11) == 0)&&(Check_BuffEntry(pt)==1))
        /*file*/
        {
            for(uint8_t index1 = 0;index1 < 8;index1++)
            {
                data.EntryName[index1] = *(pt + index1);
            }
            for(uint8_t index1 = 0;index1 < 3;index1++)
            {
                data.EntryNameExpand[index1] = *(pt + index1 + 8);
            }
            data.EntryType = *(pt+11);
            data.EntryHour = read_Hour(*(pt + 22) + *(pt + 23)*256);
            data.EntryMinute = read_Minute(*(pt + 22) + *(pt + 23)*256);
            data.EntrySecond = read_Second(*(pt + 22) + *(pt + 23)*256);
            data.EntryDay = read_Day(*(pt + 24) + *(pt + 25)*256);
            data.EntryMonth = read_Month(*(pt + 24) + *(pt + 25)*256);
            data.EntryYear = read_Year(*(pt + 24) + *(pt + 25)*256);
            data.EntryFirstCluster = ( *(pt+26)+*(pt+27)*256);
            data.EntrySize = *(pt+31)*256*256*256+*(pt+30)*256*256+*(pt+29)*256+*(pt+28);
            *head = addTail(*head,data);
            (*countEntry)++;
        }
        else if((*(pt + 11) == 16)&&(Check_BuffEntry(pt)==1))
        /*folder*/
        {
            for(uint8_t index1 = 0;index1<8;index1++)
            {
                data.EntryName[index1] = *(pt + index1);
            }
            data.EntryType = *(pt+11);
            data.EntryHour = read_Hour(*(pt + 22) + *(pt + 23)*256);
            data.EntryMinute = read_Minute(*(pt + 22) + *(pt + 23)*256);
            data.EntrySecond = read_Second(*(pt + 22) + *(pt + 23)*256);
            data.EntryDay = read_Day(*(pt + 24) + *(pt + 25)*256);
            data.EntryMonth = read_Month(*(pt + 24) + *(pt + 25)*256);
            data.EntryYear = read_Year(*(pt + 24) + *(pt + 25)*256);
            data.EntryFirstCluster = ( *(pt+26)+*(pt+27)*256);
            data.EntrySize = *(pt+31)*256*256*256+*(pt+30)*256*256+*(pt+29)*256+*(pt+28);
            *head = addTail(*head,data);
            (*countEntry)++;
        }
        index++;
    }
    firstCluster = CalculFat12(firstCluster,buffFAT);
    /*read next first cluster*/
    }
    free(buff);
    free(buffFAT);
}
void FATFS_ReadFile(uint32_t Position,FATFS_Node **head,uint8_t *BuffFile)
{
    uint8_t *BuffFAT = NULL;
    /* Allocates a buffer containing the data of fat */
    uint8_t BuffCluster[BootInfo.SectorPerCluster*BootInfo.bytePerSector];
    uint32_t temp = 0;
    uint32_t index = 0;
    uint32_t indexBuffFile = 0;
    FATFS_Node *tempNode = *head;
    uint32_t firstCluster;
    int32_t check = 0;

    BuffFAT = malloc(BootInfo.bytePerSector*BootInfo.SectorPerFat);
    for(index = 0;index < Position;index++)
    {
        tempNode = tempNode->next;
    }
    firstCluster = tempNode->data.EntryFirstCluster;
    /* find the first cluster of file */
    check = HAL_ReadSector((firstCluster+BootInfo.SectorPerFat*BootInfo.NumberFat + BootInfo.SectorPerBoots + BootInfo.EntryPerRoot*32/BootInfo.bytePerSector - 2),BuffCluster);
    //printf("%d %d",check,BootInfo.bytePerSector);
    if( check!= (int32_t)BootInfo.bytePerSector)
    {
        printf("False read File");
    }
    check = HAL_ReadMultiSector(BootInfo.SectorPerBoots,BootInfo.SectorPerFat,BuffFAT);
    if( check!= BootInfo.bytePerSector*BootInfo.SectorPerFat)
    {
        printf("False read File");
    }
    while(firstCluster != 4095)
    /* != 0xFFF*/
    {
        for(uint32_t index = 0; index < (BootInfo.bytePerSector) ; index++)
        /* assign each value to a buff containing the entire data of the file */
        {
            *(BuffFile + indexBuffFile*BootInfo.bytePerSector + index) = BuffCluster[index];
        }
        indexBuffFile++;
        temp = firstCluster;
        firstCluster = CalculFat12(temp,BuffFAT);
        check = HAL_ReadSector((firstCluster + BootInfo.SectorPerFat*BootInfo.NumberFat + BootInfo.SectorPerBoots + BootInfo.EntryPerRoot*32/BootInfo.bytePerSector - 2),BuffCluster);
    }
    free(BuffFAT);
}

/*******************************************************************************
*End of file
*******************************************************************************/


