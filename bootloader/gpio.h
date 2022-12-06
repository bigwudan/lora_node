#ifndef _GPIO_H_
#define _GPIO_H_


#define RADIO_NSS_PIN       GPIO_Pin_4
#define RADIO_NSS_PORT      GPIOA

#define RADIO_MOSI_PIN      GPIO_Pin_7
#define RADIO_MOSI_PORT     GPIOA
#define RADIO_MOSI_AF       GPIO_PinSource7

#define RADIO_MISO_PIN      GPIO_Pin_6
#define RADIO_MISO_PORT     GPIOA
#define RADIO_MISO_AF       GPIO_PinSource6

#define RADIO_SCK_PIN       GPIO_Pin_5
#define RADIO_SCK_PORT      GPIOA
#define RADIO_SCK_AF        GPIO_PinSource5

#define RADIO_nRESET_PIN    GPIO_Pin_1
#define RADIO_nRESET_PORT   GPIOF

#define RADIO_BUSY_PIN      GPIO_Pin_1
#define RADIO_BUSY_PORT     GPIOB

#define RADIO_DIO1_PIN      GPIO_Pin_0
#define RADIO_DIO1_PORT     GPIOA
 
#define RADIO_DIO3_PIN      GPIO_Pin_1
#define RADIO_DIO3_PORT     GPIOA






void GPIO_int(void);
void LowPowerGPIO_int(void);

#endif
