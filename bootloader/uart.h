#ifndef __UART_H
#define __UART_H
#include "stm32f0xx.h"
#include <stdio.h>

#define USER_UART_TX_PIN                  GPIO_Pin_2
#define USER_UART_RX_PIN                  GPIO_Pin_3

#define USER_UART_PORT                    GPIOA
#define USER_UART_GPIO_CLK           RCC_AHBPeriph_GPIOA



void UartInit(uint32_t BaudRate);
int fputc(int ch, FILE *f);
void UART_send_byte(uint8_t byte);
void UART_Send(uint8_t *Buffer, uint32_t Length);
uint8_t UART_Recive(void);


#endif

