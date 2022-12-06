#include "uart.h"
#include <stdarg.h>

               
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       

int fputc(int ch, FILE *f)
{      
	while((USART1->ISR&0X40)==0);
    USART1->TDR = (uint8_t) ch;      
	return ch;
}

void UartInit(uint32_t BaudRate)
  {  

        GPIO_InitTypeDef  GPIO_InitStructure;
        USART_InitTypeDef USART_InitStructure;
                
        RCC_AHBPeriphClockCmd( USER_UART_GPIO_CLK, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE );
                
        GPIO_PinAFConfig(USER_UART_PORT,GPIO_PinSource2,GPIO_AF_1);
        GPIO_PinAFConfig(USER_UART_PORT,GPIO_PinSource3,GPIO_AF_1);        
        /*
        *  USART1_TX -> PA9 , USART1_RX ->        PA10
        */                                
        GPIO_InitStructure.GPIO_Pin = USER_UART_TX_PIN|USER_UART_RX_PIN;                 
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
        GPIO_Init(USER_UART_PORT, &GPIO_InitStructure);        
        
        USART_InitStructure.USART_BaudRate = BaudRate;
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        USART_Init(USART1, &USART_InitStructure); 

        USART_Cmd(USART1, ENABLE);

		}			

void UART_send_byte(uint8_t byte) 
{
 while(!((USART1->ISR)&(1<<7)));
 USART1->TDR=byte;	
}		

void UART_Send(uint8_t *Buffer, uint32_t Length)
{
	while(Length != 0)
	{
		while(!((USART1->ISR)&(1<<7)));
		USART1->TDR= *Buffer;
		Buffer++;
		Length--;
	}
}

uint8_t UART_Recive(void)
{	
	while(!(USART1->ISR & (1<<5)));
	return(USART1->RDR);			 
}

		
