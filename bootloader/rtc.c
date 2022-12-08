#include "uart.h"
#include <stdarg.h>
#include "rtc.h"
               
/*RTC??????*/
void rtc_set(void)
{
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


	//RTC_InitStructure.RTC_HourFormat=RTC_HourFormat_12;
	RTC_InitStructure.RTC_SynchPrediv=0x18F;//399
	RTC_InitStructure.RTC_AsynchPrediv=0x63;//99
		RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24; 
    RTC_Init(&RTC_InitStructure);

		
	RTC_TimeTypeDef	  RTC_TimeStructure;

//YYMMDD
	RTC_DateTypeDef RTC_DateStructure;
	RTC_DateStructure.RTC_Year=21;
	RTC_DateStructure.RTC_Month=RTC_Month_January;
	RTC_DateStructure.RTC_Date=1;
	RTC_DateStructure.RTC_WeekDay=RTC_Weekday_Monday;
	RTC_SetDate(RTC_Format_BIN,&RTC_DateStructure);
	RTC_DateStructInit(&RTC_DateStructure);	
//HHMMSS	
	RTC_TimeStructure.RTC_H12=RTC_H12_AM;
	RTC_TimeStructure.RTC_Hours=0;
	RTC_TimeStructure.RTC_Minutes=0;
	RTC_TimeStructure.RTC_Seconds=0;
	RTC_SetTime(RTC_Format_BIN,&RTC_TimeStructure);

  RTC_GetTime(RTC_Format_BIN,&RTC_TimeStructure);//?????????
}


void rtc_get_time(RTC_TimeTypeDef	  *RTC_TimeStructure){

	RTC_GetTime(RTC_Format_BIN,RTC_TimeStructure);
}


void rtc_set_time(RTC_TimeTypeDef	  *RTC_TimeStructure)
{

		
//	RTC_TimeTypeDef	  RTC_TimeStructure;

////HHMMSS	
//	RTC_TimeStructure.RTC_H12=RTC_H12_AM;
//	RTC_TimeStructure.RTC_Hours=10;
//	RTC_TimeStructure.RTC_Minutes=3;
//	RTC_TimeStructure.RTC_Seconds=1;
	RTC_SetTime(RTC_Format_BCD,RTC_TimeStructure);


}

//获得秒, 分钟计数
uint32_t rtc_get_sec(){
	RTC_TimeTypeDef	  RTC_TimeStructure;
	uint32_t sec = 0;
	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStructure);
	sec = RTC_TimeStructure.RTC_Hours*60*60 + RTC_TimeStructure.RTC_Minutes*60 + RTC_TimeStructure.RTC_Seconds;
	printf("rtc_get_sec[%d][%d][%d]\n", 
					RTC_TimeStructure.RTC_Hours, RTC_TimeStructure.RTC_Minutes, RTC_TimeStructure.RTC_Seconds);
	return sec;
}
//获得秒, 分钟计数
uint32_t rtc_get_min(){
	RTC_TimeTypeDef	  RTC_TimeStructure;
	uint32_t min = 0;
	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStructure);
	min =RTC_TimeStructure.RTC_Hours*60 + RTC_TimeStructure.RTC_Minutes;
	return min;
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
	RTC_AlarmTypeDef RTC_AlarmStructure = {0};
	
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
	
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = 5;
#if 0		
	RTC_AlarmStructure.RTC_AlarmTime.RTC_H12=RTC_H12_AM;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours=RTC_TimeStructure.RTC_Hours;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes=RTC_TimeStructure.RTC_Minutes;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds=RTC_TimeStructure.RTC_Seconds+5;

	if(RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds >=60)
		RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds-=60;
 	RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x31;
 	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
#endif	
 	RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay | RTC_AlarmMask_Hours|RTC_AlarmMask_Seconds;  
 	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);
	RTC_ClearITPendingBit(RTC_IT_ALRA); 
	
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);/* Enable the alarm */
} 

/*
闹铃
@param min 分钟
*/
void RTC_Alarm_set_min(uint8_t min)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;
	uint32_t sec_time = 0;
	RTC_AlarmTypeDef RTC_AlarmStructure = {0};

	

 
	/* Enable the RTC Wakeup Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
 
	PWR_BackupAccessCmd(ENABLE);
	RTC_WriteProtectionCmd(DISABLE);	
	
	//清理中断
	RTC_ClearITPendingBit(RTC_IT_ALRA); 
	RTC_ClearITPendingBit(RTC_IT_WUT);
	RTC_ClearFlag(RTC_FLAG_ALRAF);
	
	PWR_ClearFlag(PWR_FLAG_WU);
//	PWR_ClearFlag(PWR_FLAG_SB);	
	
	
	RTC_ITConfig( RTC_IT_ALRA, DISABLE );  
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);
  
	
	
	RTC_AlarmStructInit(&RTC_AlarmStructure);
	sec_time = rtc_get_sec();
	sec_time += min*60;
	


	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours = sec_time/3600;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = sec_time%3600/60;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = sec_time%3600%60;	

	printf("cur[%d],alarm[%d][%d][%d]\n",
					sec_time,	
					RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours,
					RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes,
					RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds
					);	
 	RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;  
 	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);
	


	
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);/* Enable the alarm */
	RTC_WriteProtectionCmd(ENABLE);
	PWR_BackupAccessCmd(DISABLE);
} 

