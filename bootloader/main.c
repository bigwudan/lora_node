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
#define USER_APP1_BEGIN_ADDR 0x8002000 //定义app1程序开始地址
typedef void (*APP_FUNCTION )(void); //定义一个跳转函数模型？

void SysClock_48()
{ 
   RCC_PLLCmd(DISABLE);
   RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12);//48M
   RCC_PLLCmd(ENABLE);
   while(!RCC_GetFlagStatus(RCC_FLAG_PLLRDY));
   RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
}

void HW_int(void)
{
    SysClock_48(); 
}

void JumpApp(void)
{
	uint32_t jump_addr = *((__IO uint32_t *)(USER_APP1_BEGIN_ADDR + 4));//+4原因是前4个字节是复位函数
	APP_FUNCTION jump2app = (APP_FUNCTION)jump_addr;

	__set_MSP(*(__IO uint32_t*)USER_APP1_BEGIN_ADDR );
	jump2app();
}


int main(void){
	HW_int();//吴丹


#if 1	
		//关闭中断
	__disable_irq();
	
	JumpApp();
#endif
}




