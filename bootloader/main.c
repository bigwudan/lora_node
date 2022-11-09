#include <string.h>
#include "stm32f0xx.h"
#define MAIN_USER_FLASH_BEGIN 0x8002000


typedef void (*RESET_FUNCTION )(void); //复位函数模型

void SysClock_48()
{ 

	RCC_DeInit();
	RCC_HSICmd(ENABLE);
	
	while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == 0){
		__NOP;
	
	};
    /* Enable Prefetch Buffer and set Flash Latency */
    FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY; 
    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;      
    /* PCLK = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE_DIV1;	
	  RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12);//48M	
		RCC_PLLCmd(ENABLE);	
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == 0){
			__NOP;	
		};
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);		
		while(RCC_GetSYSCLKSource() !=RCC_CFGR_SWS_PLL ){
			__NOP;		
		}

}


static void _Delay_Ms(uint32_t delay )
{
  uint32_t i=0;
  uint32_t j=0;
  
  for(i=0;i<delay;i++)
  {
    for(j=0;j<4540;j++);
  }
}



void HW_int(void)
{

    SysClock_48(); 

		
}

void jump_app(){

}




int main(void){
	_Delay_Ms(1000*4);
	HW_int();//吴丹
	
	//关闭中断
	__disable_irq();
	
	//关闭nvic
	NVIC_DisableIRQ(WWDG_IRQn);
	
   uint32_t jump_addr=*((__IO uint32_t *)(MAIN_USER_FLASH_BEGIN+4));
   RESET_FUNCTION Reset=(RESET_FUNCTION)jump_addr;
   __set_MSP(*(__IO uint32_t*)MAIN_USER_FLASH_BEGIN);
	Reset();
	while(1){
		__NOP;
	}
}


