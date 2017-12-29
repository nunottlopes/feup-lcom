#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/driver.h>
#include <minix/com.h>

#include "rtc.h"


int rtc_get_time(unsigned long *hour, unsigned long *min)
{
	asm("cli"); //disable rtc interrupts

	sys_outb(RTC_ADDR_REG, RTC_HOURS);
	sys_inb(RTC_DATA_REG, hour);

	sys_outb(RTC_ADDR_REG, RTC_MINUTES);
	sys_inb(RTC_DATA_REG, min);

	*min = ((*min >> 4) * 10) + (*min & 0x0F);;
	*hour = ((*hour >> 4) * 10) + (*hour & 0x0F);;

	asm("sti"); //enable rtc interrupts

	return 0;
}

int rtc_get_date(unsigned long *dia, unsigned long *mes, unsigned long *ano)
{
	asm("cli"); //disable rtc interrupts

	sys_outb(RTC_ADDR_REG, RTC_DAY);
	sys_inb(RTC_DATA_REG, dia);

	sys_outb(RTC_ADDR_REG, RTC_MONTH);
	sys_inb(RTC_DATA_REG, mes);

	sys_outb(RTC_ADDR_REG, RTC_YEAR);
	sys_inb(RTC_DATA_REG, ano);

	*dia = ((*dia >> 4) * 10) + (*dia & 0x0F);;
	*mes = ((*mes >> 4) * 10) + (*mes & 0x0F);;
	*ano = ((*ano >> 4) * 10) + (*ano & 0x0F);;

	asm("sti"); //enable rtc interrupts

	return 0;
}

/*
void rtc_test_conf()
{
	unsigned long sec, min, hour, dia, mes, ano;

	rtc_get_time(&hour, &min, &sec);
	rtc_get_date(&dia, &mes,  &ano);

	printf("Date: %d-%d-%d\n", dia, mes, ano);
	printf("Time: %d:%d:%d\n", hour, min, sec);
}*/


