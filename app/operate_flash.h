#ifndef __OPERATE_FLASH_H
#define __OPERATE_FLASH_H
#include "stm32f0xx.h"
#include <stdio.h>

uint8_t operate_flash_write(uint16_t data);

uint16_t operate_flash_read();

#endif

