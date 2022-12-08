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
  RTC_GetTime(RTC_HourFormat_24,&RTC_TimeStructure);//?????????
}


void rtc_get_time(RTC_TimeTypeDef	  *RTC_TimeStructure){

	RTC_GetTime(RTC_HourFormat_12,RTC_TimeStructure);
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

