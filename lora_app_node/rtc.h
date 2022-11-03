#ifndef __RTC_H
#define __RTC_H
#include "stm32f0xx.h"
#include <stdio.h>


void rtc_get_time(RTC_TimeTypeDef	  *RTC_TimeStructure);
void rtc_set(void);

#endif

