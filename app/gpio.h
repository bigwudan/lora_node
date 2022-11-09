#ifndef _GPIO_H_
#define _GPIO_H_


#define RADIO_NSS_PIN       GPIO_Pin_12
#define RADIO_NSS_PORT      GPIOB

#define RADIO_MOSI_PIN      GPIO_Pin_15
#define RADIO_MOSI_PORT     GPIOB
#define RADIO_MOSI_AF       GPIO_PinSource15

#define RADIO_MISO_PIN      GPIO_Pin_14
#define RADIO_MISO_PORT     GPIOB
#define RADIO_MISO_AF       GPIO_PinSource14

#define RADIO_SCK_PIN       GPIO_Pin_13
#define RADIO_SCK_PORT      GPIOB
#define RADIO_SCK_AF        GPIO_PinSource13

#define RADIO_nRESET_PIN    GPIO_Pin_2
#define RADIO_nRESET_PORT   GPIOB

#define RADIO_BUSY_PIN      GPIO_Pin_10
#define RADIO_BUSY_PORT     GPIOB

#define RADIO_DIO1_PIN      GPIO_Pin_0
#define RADIO_DIO1_PORT     GPIOA
 
#define RADIO_DIO3_PIN      GPIO_Pin_0
#define RADIO_DIO3_PORT     GPIOB


#define RADIO_TX_PIN       GPIO_Pin_1
#define RADIO_TX_PORT      GPIOB


#define RADIO_RX_PIN       GPIO_Pin_6
#define RADIO_RX_PORT      GPIOA
    
#define LED1_PORT          GPIOA
#define LED1_PIN           GPIO_Pin_15



void GPIO_int(void);
void LowPowerGPIO_int(void);

#endif
