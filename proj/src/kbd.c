#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <minix/driver.h>

#include "kbd.h"
#include "i8042.h"

//Keyboard functions

static int kbdhook = 0x01;

int kbd_subscribe_int() {
	int kbdhooktemporario = kbdhook;

	if (sys_irqsetpolicy(KBC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, & kbdhook) == OK)
		if (sys_irqenable(& kbdhook) == OK)
			return BIT(kbdhooktemporario);

	return -1;
}

int kbd_unsubscribe_int() {
	if (sys_irqdisable(& kbdhook) == OK)
		if (sys_irqrmpolicy(& kbdhook) == OK)
			return 0;

	return -1;
}

unsigned long kbd_test_scan_C(){

	unsigned long store;
	sys_inb(OUT_BUF, &store);
	return store;
}
