#ifndef __RTC_H
#define __RTC_H
#include "stm32f0xx.h"
#include "stm32f0xx_rtc.h"
#include <stdio.h>


void rtc_get_time(RTC_TimeTypeDef	  *RTC_TimeStructure);
void rtc_set(void);
void RTC_Alarm_Config(void);
/*
闹铃
@param min 分钟
*/
void RTC_Alarm_set_min(uint8_t min);

#endif

