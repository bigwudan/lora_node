#ifndef _USER_H_
#define _USER_H_

#include "stm32f0xx.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "gpio.h"
#include "delay.h"
#include "spi.h"

#include "radio.h"
#include "sx126x.h"
#include "sx126x-board.h"
#include "crc.h"


#define MY_PRINTF(a,b) do{\
	for(int i=0; i< b; i++){\
		printf("[%02X]", a[i]);\
	}\
	printf("\n");\
}while(0)

#endif

