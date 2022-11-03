#include "stm32f0xx.h"
#include "gpio.h"

void GPIO_int()
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  /****************************************
   RF_NSS
  ****************************************/
  GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed=GPIO_Speed_Level_2;
  GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_Pin=RADIO_NSS_PIN;
  GPIO_Init(RADIO_NSS_PORT, &GPIO_InitStruct);
  
  GPIO_WriteBit( RADIO_NSS_PORT, RADIO_NSS_PIN,Bit_SET);
  
  /****************************************
   RF_RST
  ****************************************/
  GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed=GPIO_Speed_Level_2;
  GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_Pin=RADIO_nRESET_PIN;
  GPIO_Init(RADIO_nRESET_PORT, &GPIO_InitStruct);
  
 
  
  /****************************************
   RF_DIO1
  ****************************************/
  GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed=GPIO_Speed_Level_2;
  GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_Pin=RADIO_DIO1_PIN;
  GPIO_Init(RADIO_DIO1_PORT, &GPIO_InitStruct);
  
  
  /****************************************
   RF_DIO3
  ****************************************/
  GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed=GPIO_Speed_Level_2;
  GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_Pin=RADIO_DIO3_PIN;
  GPIO_Init(RADIO_DIO3_PORT, &GPIO_InitStruct);
  
  /****************************************
   Radio_BUSY
  ****************************************/
  GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed=GPIO_Speed_Level_2;
  GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_Pin=RADIO_BUSY_PIN;
  GPIO_Init(RADIO_BUSY_PORT, &GPIO_InitStruct);
  
  
  /****************************************
   M_CLK
  ****************************************/
  GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed=GPIO_Speed_Level_2;
  GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Pin=RADIO_SCK_PIN;
  GPIO_Init(RADIO_SCK_PORT, &GPIO_InitStruct);
  GPIO_PinAFConfig(RADIO_SCK_PORT,RADIO_SCK_AF,GPIO_AF_0);
    
  /****************************************
   M_MOSI
  ****************************************/
  GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed=GPIO_Speed_Level_2;
  GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Pin=RADIO_MOSI_PIN;
  GPIO_Init(RADIO_MOSI_PORT, &GPIO_InitStruct);
  GPIO_PinAFConfig(RADIO_MOSI_PORT,RADIO_MOSI_AF,GPIO_AF_0);

  /****************************************
   M_MISO
  ****************************************/
  GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed=GPIO_Speed_Level_2;
  GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Pin=RADIO_MISO_PIN;
  GPIO_Init(RADIO_MISO_PORT, &GPIO_InitStruct);
  GPIO_PinAFConfig(RADIO_MISO_PORT,RADIO_MISO_AF,GPIO_AF_0);
  
  
  /****************************************
   TX
  ****************************************/
  GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed=GPIO_Speed_Level_2;
  GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_Pin=RADIO_TX_PIN;
  GPIO_Init(RADIO_TX_PORT, &GPIO_InitStruct);
  
  
  /****************************************
   LED1
  ****************************************/
  GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed=GPIO_Speed_Level_2;
  GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_Pin=LED1_PIN;
  GPIO_Init(LED1_PORT, &GPIO_InitStruct);
  
}

