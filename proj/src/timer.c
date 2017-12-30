#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/driver.h>
#include <minix/com.h>

#include "timer.h"
#include "i8254.h"

static int hook = 0x03;


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
