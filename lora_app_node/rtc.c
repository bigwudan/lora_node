#include "uart.h"
#include <stdarg.h>
#include "rtc.h"
 
void RTC_NVIC_Config(void);

/*RTC??????*/
void rtc_set(void)
{

#if 1	
	
	/*RTC?????,??????????*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	/* Enable Access To The RTC & Backup Registers */
	PWR_BackupAccessCmd(ENABLE);
	RCC_LSICmd(ENABLE);  /*??LSI??*/
	while(RESET==RCC_GetFlagStatus(RCC_FLAG_LSIRDY)){} /*??LSI??*/
		
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);/*??RTC??:??LSI??*/		
	RCC_RTCCLKCmd(ENABLE);   /*??RTC??   */
	RTC_WaitForSynchro();    /*??RTC??????RTC?APB??????*/
		
	RTC_InitTypeDef RTC_InitStructure;
#if 1	
  /*
	 RTC?????3?:
	 > LSE(32.768kHz)
	 > LSI(40kHz)
	 > HSE/32?
	 > ?????:Tsec=LSI/[(RTC_AsynchPrediv+1)*(RTC_SynchPrediv +1)]
	*/
	//RTC_InitStructure.RTC_HourFormat=RTC_HourFormat_12;
	RTC_InitStructure.RTC_SynchPrediv=0x18F;//399
	RTC_InitStructure.RTC_AsynchPrediv=0x63;//99
    RTC_Init(&RTC_InitStructure);
#else	
    RTC_StructInit(&RTC_InitStructure);	//????,?????LSE(32.768KHz) 
#endif	
		
	RTC_TimeTypeDef	  RTC_TimeStructure;
#if 1
//YYMMDD
	RTC_DateTypeDef RTC_DateStructure;
	RTC_DateStructure.RTC_Year=21;
	RTC_DateStructure.RTC_Month=RTC_Month_August;
	RTC_DateStructure.RTC_Date=1;
	RTC_DateStructure.RTC_WeekDay=RTC_Weekday_Monday;
	RTC_SetDate(RTC_Format_BCD,&RTC_DateStructure);
	RTC_DateStructInit(&RTC_DateStructure);	
//HHMMSS	
	RTC_TimeStructure.RTC_H12=RTC_H12_AM;
	RTC_TimeStructure.RTC_Hours=10;
	RTC_TimeStructure.RTC_Minutes=3;
	RTC_TimeStructure.RTC_Seconds=1;
	RTC_SetTime(RTC_Format_BCD,&RTC_TimeStructure);
#else
	RTC_TimeStructInit(&RTC_TimeStructure);/*???? (Time = 00h:00min:00sec).*/
#endif
  RTC_GetTime(RTC_HourFormat_12,&RTC_TimeStructure);//?????????
#endif

	RTC_Alarm_Config();
}


void rtc_get_time(RTC_TimeTypeDef	  *RTC_TimeStructure){

	RTC_GetTime(RTC_HourFormat_12,RTC_TimeStructure);
}

#define u8 uint8_t

/*
	RTC初始化
*/
u8 RTC_Init_rtc(void)
{
	RTC_NVIC_Config();
	
	/*RTC?????,??????????*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	/* Enable Access To The RTC & Backup Registers */
	PWR_BackupAccessCmd(ENABLE);
	RCC_LSICmd(ENABLE);  /*??LSI??*/
	while(RESET==RCC_GetFlagStatus(RCC_FLAG_LSIRDY)){} /*??LSI??*/
		
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);/*??RTC??:??LSI??*/		
	RCC_RTCCLKCmd(ENABLE);   /*??RTC??   */
	RTC_WaitForSynchro();    /*??RTC??????RTC?APB??????*/
		
	RTC_InitTypeDef RTC_InitStructure;
#if 1	
  /*
	 RTC?????3?:
	 > LSE(32.768kHz)
	 > LSI(40kHz)
	 > HSE/32?
	 > ?????:Tsec=LSI/[(RTC_AsynchPrediv+1)*(RTC_SynchPrediv +1)]
	*/
	//RTC_InitStructure.RTC_HourFormat=RTC_HourFormat_12;
	RTC_InitStructure.RTC_SynchPrediv=0x18F;//399
	RTC_InitStructure.RTC_AsynchPrediv=0x63;//99
    RTC_Init(&RTC_InitStructure);
#else	
    RTC_StructInit(&RTC_InitStructure);	//????,?????LSE(32.768KHz) 
#endif	
		
	RTC_TimeTypeDef	  RTC_TimeStructure;
#if 1
//YYMMDD
	RTC_DateTypeDef RTC_DateStructure;
	RTC_DateStructure.RTC_Year=21;
	RTC_DateStructure.RTC_Month=RTC_Month_August;
	RTC_DateStructure.RTC_Date=1;
	RTC_DateStructure.RTC_WeekDay=RTC_Weekday_Monday;
	RTC_SetDate(RTC_Format_BCD,&RTC_DateStructure);
	RTC_DateStructInit(&RTC_DateStructure);	
//HHMMSS	
	RTC_TimeStructure.RTC_H12=RTC_H12_AM;
	RTC_TimeStructure.RTC_Hours=10;
	RTC_TimeStructure.RTC_Minutes=3;
	RTC_TimeStructure.RTC_Seconds=1;
	RTC_SetTime(RTC_Format_BCD,&RTC_TimeStructure);
#else
	RTC_TimeStructInit(&RTC_TimeStructure);/*???? (Time = 00h:00min:00sec).*/
#endif
  RTC_GetTime(RTC_HourFormat_12,&RTC_TimeStructure);//?????????		


	RTC_ITConfig(RTC_IT_ALRA, ENABLE);		//使能RTC时钟中断

	return 0; //ok
}	
 

 
void RTC_NVIC_Config(void)
{	
  NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	
	//EXTI17配置
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;         // RTC闹钟为外部中断17
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	// 时钟中断配置
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;		//RTC全局中断
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;	//先占优先级1位,从优先级3位

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//使能该通道中断
	NVIC_Init(&NVIC_InitStructure);		//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}

void RTC_Alarm_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;
	RTC_TimeTypeDef   RTC_TimeStructure;
	RTC_AlarmTypeDef RTC_AlarmStructure;
	
	/* EXTI configuration *******************************************************/
//	EXTI_ClearITPendingBit(EXTI_Line17);
//	EXTI_InitStructure.EXTI_Line = EXTI_Line17;
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);
 
	/* Enable the RTC Wakeup Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
 
	RTC_ITConfig( RTC_IT_ALRA, DISABLE );  
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);
  
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	RTC_AlarmStructure.RTC_AlarmTime.RTC_H12=RTC_H12_AM;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours=RTC_TimeStructure.RTC_Hours;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes=RTC_TimeStructure.RTC_Minutes;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds=RTC_TimeStructure.RTC_Seconds+5;
	if(RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds >=60)
		RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds-=60;
 	RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x31;
 	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
 	RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay | RTC_AlarmMask_Hours|RTC_AlarmMask_Minutes;  
 	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);
	RTC_ClearITPendingBit(RTC_IT_ALRA); 
	
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);/* Enable the alarm */
} 
