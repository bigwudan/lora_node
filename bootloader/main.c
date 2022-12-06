/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2016 Semtech

Description: Tx Continuous Wave implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
#include <string.h>
#include "stm32f0xx.h"
#include "uart.h"
#include "spi.h"
#include "gpio.h"

#include "radio.h"
#include "sx126x.h"
#include "sx126x-board.h"
#include "crc.h"

#include "lora_common.h"




void SysClock_48()
{ 
		RCC_DeInit();
   RCC_PLLCmd(DISABLE);
   RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12);//48M
   RCC_PLLCmd(ENABLE);
   while(!RCC_GetFlagStatus(RCC_FLAG_PLLRDY));
   RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
}


void RCC_Configuration()
{   
  /* Enable GPIO clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOB|RCC_AHBPeriph_GPIOC|RCC_AHBPeriph_GPIOF, ENABLE);
  
  /* Enable peripheral Clock */

  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1|RCC_APB2Periph_SYSCFG, ENABLE);
}

void Tick_Configration()
{
    /* Setup SysTick Timer for 1ms interrupts ( not too often to save power ) */
    if( SysTick_Config( SystemCoreClock / 1000 ) )
    { 
        /* Capture error */ 
        while (1);
    }
}


void HW_int(void)
{
	SysClock_48(); 
	UartInit(9600);
	Tick_Configration();
	RCC_Configuration();
	GPIO_int();
	SPI1_Int();
}



int main( void )
{   
	HW_int();//MCU外围资源初始化
	printf("main_ok\n");
	lora_node_init(); //初始化
	printf("lora_node_init_ok\n");
	while(1){
		Radio.IrqProcess( ); // Process Radio IRQ
		lora_node_task();		
		//printf("xxxxx[%d][%d][%d]\n", RTC_TimeStructure.RTC_Hours, RTC_TimeStructure.RTC_Minutes, RTC_TimeStructure.RTC_Seconds);
	}
}





