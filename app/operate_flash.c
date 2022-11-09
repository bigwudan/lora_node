#include "operate_flash.h"
#include <stdarg.h>

#define FLASH_USER_START_ADDR 0x0800FC00
               
uint8_t operate_flash_write(uint16_t data){
  FLASH_Unlock();
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR); 
	if (FLASH_ErasePage(FLASH_USER_START_ADDR )!= FLASH_COMPLETE)
	{
		return -1;
	}
	if (FLASH_ProgramHalfWord(FLASH_USER_START_ADDR, data) != FLASH_COMPLETE) 
	{
		return -1;
	}
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR); 
	FLASH_Lock();
  return 0;
}

uint16_t operate_flash_read()
{
	uint16_t data = *(uint16_t*)(FLASH_USER_START_ADDR);
	return data;
}	
	