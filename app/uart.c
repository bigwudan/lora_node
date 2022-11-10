#include "uart.h"
#include <stdarg.h>

#define UART1_RX_LEN 16

//#define UART1_RX_LEN 16
 
#define TX_CHANNEL 	DMA1_Channel2 
#define RX_CHANNEL 	DMA1_Channel3 

struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       

static uint8_t t_buf[16] = {0x11};

uint8_t rx_buf[UART1_RX_LEN] = {0};

int fputc(int ch, FILE *f)
{
#if 0	
	while((USART1->ISR&0X40)==0);
    USART1->TDR = (uint8_t) ch;
#else
	uint8_t t_buf[2] = {0};
	t_buf[0] = ch;
	UartDMA_send(t_buf, 1);
	while(DMA_GetFlagStatus(DMA1_FLAG_TC2) == RESET){
		__NOP;
	}

#endif	
	return ch;
}

void UartDMAInit(){

	
	//使能dma时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	DMA_InitTypeDef DMA_InitStructure;
	
	DMA_DeInit(TX_CHANNEL);	
	DMA_Cmd(TX_CHANNEL, DISABLE);
	DMA_InitStructure.DMA_PeripheralBaseAddr 	= (uint32_t)&USART1->TDR;//外设数据地址
	DMA_InitStructure.DMA_MemoryBaseAddr 		= (uint32_t)t_buf;  //自己的接收buf
	DMA_InitStructure.DMA_BufferSize			= UART1_RX_LEN;  // 缓存大小
	DMA_InitStructure.DMA_M2M 					= DMA_M2M_Disable;	 // 内存到内存关闭
	DMA_InitStructure.DMA_Mode 					= DMA_Mode_Normal;	// 普通模式
	DMA_InitStructure.DMA_DIR 					= DMA_DIR_PeripheralDST;	 // 外设到内存
	DMA_InitStructure.DMA_Priority 				= DMA_Priority_High; // DMA通道优先级
	DMA_InitStructure.DMA_MemoryInc 			= DMA_MemoryInc_Enable;// 内存地址递增
	DMA_InitStructure.DMA_PeripheralInc			= DMA_PeripheralInc_Disable;//外设基地址不需要递增
	DMA_InitStructure.DMA_MemoryDataSize 		= DMA_MemoryDataSize_Byte; 
	DMA_InitStructure.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_Byte;
	DMA_Init(TX_CHANNEL,&DMA_InitStructure);
	DMA_Cmd(TX_CHANNEL, ENABLE); 	
	
	
	
	/*****DMA接收配置 Peripheral->Memory*****************/
	DMA_DeInit(RX_CHANNEL);
	DMA_Cmd(RX_CHANNEL, DISABLE);	
	DMA_InitStructure.DMA_BufferSize = UART1_RX_LEN;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->RDR;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)rx_buf;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_Init(RX_CHANNEL,&DMA_InitStructure);
	

	DMA_Cmd(RX_CHANNEL,ENABLE); //使能DMA通道5

	
}

void UartDMA_send(uint8_t *buf, uint16_t len){
	
	if(DMA_GetFlagStatus(DMA1_FLAG_TC2) == SET){
	
		DMA_ClearFlag(DMA1_FLAG_TC2);
		
		memmove(t_buf, buf, len);
		DMA_Cmd(TX_CHANNEL, DISABLE);
		DMA_SetCurrDataCounter(TX_CHANNEL, len);
		DMA_Cmd(TX_CHANNEL, ENABLE); 
	}

	

}


void UartInit(uint32_t BaudRate)
  {  

        GPIO_InitTypeDef  GPIO_InitStructure;
        USART_InitTypeDef USART_InitStructure;
				NVIC_InitTypeDef NVIC_InitStruct;
                
        RCC_AHBPeriphClockCmd( USER_UART_GPIO_CLK, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE );
                
        GPIO_PinAFConfig(USER_UART_PORT,GPIO_PinSource9,GPIO_AF_1);
        GPIO_PinAFConfig(USER_UART_PORT,GPIO_PinSource10,GPIO_AF_1);        
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

				//USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);  
				USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
				USART_ClearFlag(USART1, USART_FLAG_IDLE);

				USART_DMACmd(USART1,USART_DMAReq_Tx, ENABLE );
				
				USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);
				
        USART_Cmd(USART1, ENABLE);




				NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
				NVIC_InitStruct.NVIC_IRQChannelPriority = 0x02;
				NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
				NVIC_Init(&NVIC_InitStruct);
				
				UartDMAInit();

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




		
