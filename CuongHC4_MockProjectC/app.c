/*******************************************************************************
*Include
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include "APP.h"
#include "FATFS.h"
/*******************************************************************************
*STATIC
*******************************************************************************/

/**
 * @brief  print the components of an entry
 *
 * @param  Entry: information of that entry
 * @return void
 */
static void PrintEntry(FATFS_Entry_Struct_t Entry);

/**
 * @brief  print the entries in Linkedlist
 *
 * @param  *head: pointer to the first node of the list
 * @pram   countEntry: number of elements of Linkedlist
 * @return void
 */
static void printList(FATFS_Node *head,uint32_t countEntry);

/**
 * @brief  Check if the input characters of the string are integers
 *
 * @param  string[]: string to check
 * @return int: 1 true
 *            : 0 false
 */
static int checkFunc(uint8_t string[]);

/**
 * @brief  convert string => integers
 *
 * @param  string: string to convert
 * @return int: value
 */
static int str2dec(uint8_t string[]);

static void PrintEntry(FATFS_Entry_Struct_t Entry)
{
    uint8_t index = 0;
    uint8_t count = 0;
    for(index = 0;index < 8;index++)
    {
        if(Entry.EntryName[index] != 32)
        /* check the space character */
        {
            printf("%c",Entry.EntryName[index]);
        }
        else
        {
            count++;
        /* number of space characters in the name */
        }
    }
    if(Entry.EntryType == 16)
    {
        printf("   ");
    }

    if(Entry.EntryType == 0)
    {
        printf(".");
    }
    for(index = 0;index<3;index++)
    {
        if(Entry.EntryNameExpand[index] != 32)
        {
            printf("%c",Entry.EntryNameExpand[index]);
        }
    }

    for(index = 0;index<(count);index++)
    {
        printf(" ");
    }
    if(Entry.EntryNameExpand[0]=='T')
    /*TXT */
    {
        printf("%21s","Text Document");
    }
    else if(Entry.EntryNameExpand[0]=='D')
    /*DOC*/
    {
        printf("%21s","Doc Document ");
    }
    else if(Entry.EntryNameExpand[0]=='P')
    /*PDF*/
    {
        printf("%21s","PDF Document ");
    }
    if(Entry.EntryType == 0)
    {
        printf("%15d",Entry.EntrySize);
        printf("%15d/%02d/%d %02d:%02d:%02d",Entry.EntryDay,Entry.EntryMonth,Entry.EntryYear,Entry.EntryHour,Entry.EntryMinute,Entry.EntrySecond);
    }
    else if(Entry.EntryType == 16)
    {
        printf("%52d/%02d/%d %02d:%02d:%02d",Entry.EntryDay,Entry.EntryMonth,Entry.EntryYear,Entry.EntryHour,Entry.EntryMinute,Entry.EntrySecond);
    }
    printf("\n");
}

static void printList(FATFS_Node *head,uint32_t countEntry)
{
    FATFS_Node *temp = NULL;
    temp = head;
    printf("%-5s","NO");
    printf("%-20s","NAME");
    printf("%-25s","TYPE");
    printf("%-20s","SIZE");
    printf("%-20s","MODIFIED");
    printf("\n");
    for(uint8_t index2 = 0;index2 < (countEntry);index2++)
    {
        printf("%-5d",index2 + 1);
        PrintEntry(temp->data);
        temp = temp->next;
    }
}

static int checkFunc(uint8_t string[])
{
    int result = 1;
    for(int index = 0;index < (int)strlen((char*)string);index++)
    {
        if((string[index]>'9')||(string[index]<'0'))
        {
            result = 0;
        }
    }
    return result;
}

static int str2dec(uint8_t string[])
{
    int Dec = 0;
    if(checkFunc((uint8_t*)string) == 0)
    {
        Dec = -1;
    }
    else
    {
        for(int index = 0;index<(int)strlen((char*)string);index++)
        {
            Dec = ((int)string[index]-48)*(pow(10,(strlen((char*)string)- index - 1))) + Dec;
        }
    }
    return Dec;
}

/*******************************************************************************
*API
*******************************************************************************/
void mainAppFat12(const char *FilePath)
{
    FATFS_Node *head = NULL;
    /* pointer to first element of list */
    FATFS_Node *temp = NULL;
    FATFS_BootSector_Struct_t boot_info;
    /* variable containing information to read in bootSector */
    uint8_t *buff = NULL;
    /* pointers are intended to allocate data areas containing long file content */
    uint32_t countEntry = 0;
    /* the number of elements of the list */
    uint8_t StringOption[10] = {};
    /* string containing user selection */
    uint32_t option = 1;
    /* user's choice */
    uint8_t check = 0;
    uint32_t size = 0;

    check = FATFS_Init(FilePath,&boot_info);
    if (check == 0)
    /* error in fopen in library hal */
    {
        printf("False Init");
    }
    else
    {
        FATFS_ReadRoot((boot_info.NumberFat*boot_info.SectorPerFat+1),(boot_info.EntryPerRoot*32),&head,&countEntry);
        /* read the components of BootSector */
        printList(head,countEntry);
        /* print to the screen the list of entries contained in the RootSector */
        printf("\n======================================================================================================================\n");
        printf("Select: 1 - %d|Exit: 0",countEntry);
        while(option != 0)
        /* option = 0: exit the program */
        {
            printf("\nPlease enter option: ");
            fflush(stdin);
            gets((char*)StringOption);
            fflush(stdin);
            while((checkFunc(StringOption) == 0)||((uint32_t)str2dec(StringOption)>(countEntry))||(str2dec(StringOption)<0))
            /* check the data read in */
            {
                printf("This option is not available!\n");
                printf("Pls re-enter option: ");
                fflush(stdin);
                gets((char*)StringOption);
                fflush(stdin);
            }
            option = str2dec(StringOption);
            if(option > 0)
            {
                option--;
                /* because the display starts at 1, but in fact the elements of the Linklist are numbered from 0 */
                system("cls");
                check = FATFS_Access(option,&head);
                if(check == 0)
                /* Access root */
                {
                    system("cls");
                    FATFS_ReadRoot(boot_info.NumberFat*boot_info.SectorPerFat+1,(boot_info.EntryPerRoot*32),&head,&countEntry);
                    printList(head,countEntry);
                    printf("\n======================================================================================================================\n");
                    printf("Select: 1 - %d|Exit: 0",countEntry);
                }
                else if(check == 1)
                /* Access Sub */
                {
                    system("cls");
                    FATFS_ReadSubDirectory(option,&head,&countEntry);
                    printList(head,countEntry);
                    printf("\n======================================================================================================================\n");
                    printf("Select: 1 - %d|Exit: 0",countEntry);
                }
                else if(check == 2)
                /* access a file */
                {
                    system("cls");
                    temp = head;
                    for(uint8_t index = 0; index < option;index++)
                    {
                        temp = temp->next;
                    }
                    size = temp->data.EntrySize;
                    size = (((size/boot_info.bytePerSector)+1)*boot_info.bytePerSector);
                    /* calculate the buffer size to be allocated */
                    buff = malloc(size);
                    /* allocate a buffer containing the file's data */
                    FATFS_ReadFile(option,&head,buff);
                    for(uint32_t index1 = 0; index1 < temp->data.EntrySize;index1++)
                    {
                        printf("%c",buff[index1]);
                    }
                    printf("\n======================================================================================================================\n");
                    printList(head,countEntry);
                    printf("\n======================================================================================================================\n");
                    printf("Select: 1 - %d|Exit: 0",countEntry);
                    free(buff);
                }
                option++;
                /* returns the option to its original value */
            }
            else
            /* option = 0*/
            {
                printf("\nYou have exited the program!!!See you again.");
                FATFS_DeInit();
            }
        }
    }
}

/*******************************************************************************
*End of file
*******************************************************************************/


