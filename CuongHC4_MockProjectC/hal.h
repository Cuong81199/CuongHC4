/*******************************************************************************
* Definitions
*******************************************************************************/
#ifndef  _HAL_H_
#define  _HAL_H_

/*******************************************************************************
*API
*******************************************************************************/

/**
 * @brief  read 1 sector 1 time
 *
 * @param  index: position read sector
 * @param  buff: store data of sector
 * @return int32_t: number of bytes read| -1 or 0 : error
 */
int32_t HAL_ReadSector(uint32_t index,uint8_t *buff);

/**
 * @brief  read multiple sectors at once
 *
 * @param  index: position read first sector
 * @param  num: number of sector be readed
 * @param  buff: store data of multi sector
 * @return int32_t: number of bytes read| -1 or 0 : error
 */
int32_t HAL_ReadMultiSector(uint32_t index,uint32_t num,uint8_t *buff);

/**
 * @brief  pass in the file to read from FATFS layer
 *
 * @param  filepath
 * @return 0: Error
 */
uint8_t HAL_Init(const char *const FilePath);

/**
 * @brief  closes the file passed in the Init function
 */
void HAL_DeInit();

/**
 * @brief  update the number of bytes of the boot sector
 *
 * @param  update: update value
 * @return void
 */
void HAL_update(uint16_t update);

/*******************************************************************************
* Definitions
*******************************************************************************/
#endif /* _HAL_H_ */
/*******************************************************************************
*End of file
*******************************************************************************/
