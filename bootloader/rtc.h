#ifndef __RTC_H
#define __RTC_H
#include "stm32f0xx.h"
#include <stdio.h>


void rtc_get_time(RTC_TimeTypeDef	  *RTC_TimeStructure);
void rtc_set_time(RTC_TimeTypeDef	  *RTC_TimeStructure);
//获得秒, 分钟计数
uint32_t rtc_get_sec();

//获得秒, 分钟计数
uint32_t rtc_get_min();
void rtc_set(void);

#endif

