#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/driver.h>
#include <minix/com.h>
#include "i8254.h"
#include "timer2.h"

//Funções definidas no lab2

static int hook = 0;

int timer_get_conf(unsigned char timer, unsigned char *st) {

	unsigned char port;
	unsigned long byte;

	if(timer == 0)
		port = TIMER_0;
	else if (timer == 1)
		port = TIMER_1;
	else if (timer == 2)
		port = TIMER_2;
	else
		return 1;

	sys_outb(TIMER_CTRL, TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer));
	sys_inb(port, & byte);
	*st = (unsigned char) byte;

	return 0;
}

int timer_subscribe_int(void ) {
	int hooktemporario = hook;

	if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, & hook) == OK)
		if (sys_irqenable(& hook) == OK)
			return BIT(hooktemporario);

	return -1;
}

int timer_unsubscribe_int() {
	if (sys_irqdisable(& hook) == OK)
		if (sys_irqrmpolicy(& hook) == OK)
			return 0;

	return -1;
}

int timer_set_frequency(unsigned char timer, unsigned long freq) {

	if (timer < 0)
		return 1;
	else if (timer > 2)
		return 1;

	unsigned char config;
	unsigned long valorfreq = TIMER_FREQ/freq;
	unsigned long novovalor = valorfreq >> 8;

	if(timer_get_conf(timer, &config) != 0)
		return 1;

	unsigned char lsb = config & 0x0F;

	if (sys_outb(TIMER_CTRL, ((TIMER_0 + timer) | TIMER_LSB_MSB | lsb)) != 0)
		return 1;

	if ((sys_outb(TIMER_0 + timer, valorfreq) != 0) || (sys_outb(TIMER_0 + timer, novovalor) != 0))
		return 1;
	else
		return 0;
}


int timer_test_time_base(unsigned long freq) {

	if(timer_set_frequency(0, freq) != 0)
		return 1;
	return 0;
}
