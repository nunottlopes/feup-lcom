#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <minix/driver.h>
#include "test3.h"
#include "i8042.h"
#include "i8254.h"
#include "timer2.h"
#include "kbd.h"


int kbd_test_scan(unsigned short ass) {

	 int ipc_status, r, flag = 0;
	 message msg;
	 unsigned long type;
	 int counter = 0;

	 int irq_set = kbd_subscribe_int();

	 if (irq_set == -1)
	 		return 1;


	 while (type != ESC){
		 if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			 printf("driver_receive failed with: %d", r);
			 continue;
		 }
		 if (is_ipc_notify(ipc_status)) {
			 switch (_ENDPOINT_P(msg.m_source)) {
			 case HARDWARE:
				 if (msg.NOTIFY_ARG & irq_set) {
					 type = kbd_choosing_C_or_ASM(ass);
					 flag = kbd_print(type,flag);
					 counter++;
				 }
				 break;
			 default:
				 break;
			 }
		 }
	 }

	 if(kbd_unsubscribe_int() == -1)
		 return 1;

	 printf("Number of sys_inb kernel calls: %d\n", counter);

	 return 0;
}

int kbd_test_poll() {
    unsigned long type, stat;
    int counter = 0, flag = 0;

    do {
    	sys_inb(STAT_REG, &stat);
    	if((stat & OBF) != 0 && (stat & AUX) == 0) {
    		sys_inb(OUT_BUF, &type);
    		counter++;
    		if ( (stat &(PAR_ERR | TO_ERR)) == 0 )
    			flag = kbd_print(type,flag);
    		else
    			return -1;
    	}
    	tickdelay(micros_to_ticks(DELAY_US));
    } while(type != ESC);

    printf("Number of sys_inb kernel calls: %d\n", counter);
    return 0;
}

int kbd_test_timed_scan(unsigned short n) {

	int ipc_status, r, flag = 0;
	message msg;
	unsigned long type;

	int contador = 0, contador2 = 0;

	int irq_set = kbd_subscribe_int();
	int irq_set_t = timer_subscribe_int();

	if (irq_set == -1 || irq_set_t == -1)
		return 1;

	while ((type != ESC) && (contador < n)){
		if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_set_t) {
					contador2++;
					if ((contador2/60) == 1)
					{
						contador2 = 0;
						contador++;
					}
				}
				if (msg.NOTIFY_ARG & irq_set) {
					type = kbd_test_scan_C();
					flag = kbd_print(type,flag);
				}
				break;
			default:
				break;
			}
		}
	}

	if(kbd_unsubscribe_int() == -1)
		return 1;

	if(timer_unsubscribe_int() == -1)
		return 1;

	return 0;
}
