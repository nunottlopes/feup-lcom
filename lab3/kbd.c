#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <minix/driver.h>
#include "kbd.h"
#include "i8042.h"

//Keyboard functions

static int kbdhook = 1;
extern unsigned long kbdass();

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


unsigned long kbd_choosing_C_or_ASM(unsigned short ass){
	if(ass != 0)
		return kbdass();
	else
		return kbd_test_scan_C();
}

int kbd_print(unsigned long type, int flag){
	int m = 0;

	if (type == TWO_BYTE)
		return 1;

	 if (flag == 1)
	 {
		 if((type & 0x80) != 0x00){
			 printf("Breakcode: 0xe0 0x%x \n", type);
		 }
		 else{
			 printf("Makecode: 0xe0 0x%x \n", type);
		 }
		 flag = 0;
		 m = 1;
	 }

	 if (m == 0)
	 {
		 if ((type & 0x80) != 0x00)
			 printf("Breakcode: 0x%x \n",type);
		 else
			 printf("Makecode: 0x%x \n", type);
	 }
	 return flag;
}
