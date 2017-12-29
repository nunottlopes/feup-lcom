#ifndef __RTC_H
#define __RTC_H


#define RTC_ADDR_REG 		0x70
#define RTC_DATA_REG 		0x71
#define RTC_CTRL_REG_A 		0x0A
#define RTC_CTRL_REG_B  	0x0B
#define RTC_CTRL_REG_C  	0X0C
#define RTC_CTRL_REG_D  	0X0D
#define RTC_SECONDS 		0
#define RTC_MINUTES			2
#define RTC_HOURS 			4
#define RTC_DAY				7
#define RTC_MONTH			8
#define RTC_YEAR			9


int rtc_get_time(unsigned long *hour, unsigned long *min);

int rtc_get_date(unsigned long *dia, unsigned long *mes, unsigned long *ano);


#endif /* __RTC_H */
