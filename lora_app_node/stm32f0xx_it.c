/**
  ******************************************************************************
  * @file    Project/STM32F0xx_StdPeriph_Templates/stm32f0xx_it.c 
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    05-December-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_it.h"
#include "user.h"

volatile  uint32_t TickCounter=0;
volatile  uint32_t ticktimer=0;

volatile  uint32_t count_timer=0;

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  
  TickCounter++;
  ticktimer++;

}




/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
uint16_t rx_data = 0;
void USART1_IRQHandler(void)
{
	uint16_t data = 0;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		rx_data = USART_ReceiveData(USART1);
	}
}

void ADC1_IRQHandler(void)
{
  
}

void SPI2_IRQHandler(void)
{
  
}

void TIM14_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM14, TIM_IT_Update))
  {
    TIM_ClearITPendingBit(TIM14,TIM_IT_Update);
 
  }
}


 /* * @brief  This function handles RTC interrupt request.
  * @param  None
  * @retval None
  */


void EXTI0_1_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line1) != RESET)
  {
    /* Clear the EXTI line 1 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line1);
    
    
  }
}
void TIM3_IRQHandler(void)   
{
	static int idx = 0;
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  
      {	 
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);	
				count_timer++;
        if(count_timer==100)
        {
         count_timer=0;
         
        }
				if(idx == 0){
					idx = 1;
					GPIO_WriteBit( LED1_PORT, LED1_PIN,Bit_RESET);
				}else{
					idx = 0;
					GPIO_WriteBit( LED1_PORT, LED1_PIN,Bit_SET);
				}
      }
}

void RTC_IRQHandler(void){
	printf("RTC ALARM INTERRUPT\r\n");
	
}
