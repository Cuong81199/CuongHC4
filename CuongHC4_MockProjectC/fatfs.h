/*******************************************************************************
* Definitions
*******************************************************************************/
#ifndef  _FAFTS_H_
#define  _FATFS_H_
/*******************************************************************************
*Struct
*******************************************************************************/
typedef struct{
    uint8_t     TypeFat[8];
    uint8_t     SectorPerCluster;
    uint8_t     SectorPerBoots;
    uint8_t     NumberFat;
    uint16_t    EntryPerRoot;
    uint16_t    SectorPerFat;
    uint16_t    bytePerSector;
} FATFS_BootSector_Struct_t;

typedef struct{
    uint8_t  EntryName[8];
    uint8_t  EntryNameExpand[3];
    uint8_t  EntrySecond;
    uint8_t  EntryMinute;
    uint8_t  EntryHour;
    uint8_t  EntryDay;
    uint8_t  EntryMonth;
    uint32_t  EntryYear;
    uint32_t  EntryFirstCluster;
    uint32_t EntryFarther;
    uint8_t  EntryType;
    uint32_t EntrySize;
}FATFS_Entry_Struct_t;

struct FATFS_Node{
    FATFS_Entry_Struct_t data;
    struct FATFS_Node* next;
};

typedef struct FATFS_Node FATFS_Node;

/*******************************************************************************
*API
*******************************************************************************/
/**
 * @brief  pass values from the App layer to the FATFS layer
 *
 * @param  pt: poiter to variable boot_info in App
 * @param  filepath: pointer to the file to be processed
 * @return 0 : error
 */
uint8_t FATFS_Init(const char * const FilePath,FATFS_BootSector_Struct_t *pt);

/**
 * @brief  close the file passed in the FATFS_Init function
 */
void FATFS_DeInit();

/**
 * @brief  read required values in Boot Sector
 *
 * @param  pt: pt: poiter to variable boot_info in App
 */
void FATFS_ReadBoot(FATFS_BootSector_Struct_t *pt);

/**
 * @brief  define the object the user wants to access
 *
 * @param  position: position in linkedlist
 * @param  head: pointer to pointer to list in app
 * @return 0: root|1: folder|2: file
 */
uint8_t FATFS_Access(uint32_t Position,FATFS_Node **head);

/**
 * @brief  read and create list of emtry in root
 *
 * @param  pos: position of root sector
 * @param  head: pointer to pointer to list in app
 * @param  entrycount: number of elements of linkedlist
 * @return void
 */
void FATFS_ReadRoot(uint8_t pos,uint32_t _sizeRoot,FATFS_Node **head,uint32_t*countEntry);

/**
 * @brief  read and create list of emtry in subdirectory
 *
 * @param  position: position of empty in list
 * @param  head: pointer to pointer to list in app
 * @param  entrycount: number of elements of linkedlis
 * @return void
 */
void FATFS_ReadSubDirectory(uint32_t Position,FATFS_Node **head,uint32_t *countEntry);

/**
 * @brief  print data of file
 *
 * @param  position: position of empty in list
 * @param  head: pointer to pointer to list in app
 * @param  buffFile: store data of file
 * @return void
 */
void FATFS_ReadFile(uint32_t Position,FATFS_Node **head,uint8_t *BuffFile);

/*******************************************************************************
* Definitions
*******************************************************************************/
#endif /* _FATFS_H_ */
/*******************************************************************************
*End of file
*******************************************************************************/
