#include <string.h>
#include "stm32f0xx.h"
#include "uart.h"
#include "gpio.h"
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

void Tick_Configration()
{
    /* Setup SysTick Timer for 1ms interrupts ( not too often to save power ) */
    if( SysTick_Config( SystemCoreClock / 1000 ) )
    { 
        /* Capture error */ 
        while (1);
    }
}

void RCC_Configuration()
{   
  /* Enable GPIO clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOB|RCC_AHBPeriph_GPIOC|RCC_AHBPeriph_GPIOF, ENABLE);
  
  /* Enable peripheral Clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2|RCC_APB1Periph_PWR, ENABLE);  
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_SYSCFG, ENABLE);
}

void HW_int(void)
{

    SysClock_48(); 
    Tick_Configration();
    RCC_Configuration();
		GPIO_int();
		UartInit(9600);
		
}



//用户程序存放地址（也是存放中断向量的flash地址）
#define APPLICATION_ADDRESS ((uint32_t)0x08005000)
 

int main(void)
{
	memcpy((void*)0x20000000, (void*)APPLICATION_ADDRESS, 0xB4);
	SYSCFG_MemoryRemapConfig(SYSCFG_MemoryRemap_SRAM);
	/*user code begin*/
	//正常用户程序
	/*user code end*/
 //初始化时钟
 HW_int();
	__enable_irq();
 while(1){
		__NOP;
	 printf("wudan\n");
	 //GPIO_WriteBit( LED1_PORT, LED1_PIN,Bit_SET);
 }	
	
}


