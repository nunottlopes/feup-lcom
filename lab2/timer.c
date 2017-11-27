#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include "i8254.h"

unsigned int contador = 0;
int hook;
unsigned long frequencianormal = 60;

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

	if (sys_outb(TIMER_CTRL, TIMER_0 + timer | TIMER_LSB_MSB | lsb) != 0)
		return 1;

	if ((sys_outb(TIMER_0 + timer, valorfreq) != 0) || (sys_outb(TIMER_0 + timer, novovalor) != 0))
		return 1;
	else
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

void timer_int_handler() {
	contador++;
}

int timer_display_conf(unsigned char conf) {
	

	(0x80 & conf) ? printf("1") : printf("0");
	(0x40 & conf) ? printf("1") : printf("0");
	(0x20 & conf) ? printf("1") : printf("0");
	(0x10 & conf) ? printf("1") : printf("0");
	(0x08 & conf) ? printf("1") : printf("0");
	(0x04 & conf) ? printf("1") : printf("0");
	(0x02 & conf) ? printf("1") : printf("0");
	(0x01 & conf) ? printf("1") : printf("0");
	printf("\n");


	printf("Counting mode: ");
	if (conf & 0x01)
		printf("BCD (4 decades)\n");
	else
		printf("Binary (16 bits)\n");


	printf("Operating mode: ");
	if (conf & 0x04)
	{
		if (conf & 0x02)
			printf("3\n");
		else
			printf("2\n");
	}
	else{
		if(conf & 0x08){
			if(conf & 0x02)
				printf("5\n");
			else
				printf("4\n");
		}
		else
			if(conf & 0x02)
				printf("1\n");
			else
				printf("0\n");
	}


	printf("Type of access: ");
	if(conf & 0x20){
		if(conf & 0x10)
			printf("LSB followed by MSB\n");
		else
			printf("MSB\n");
	}
	else
		printf("LSB\n");


	printf("Select counter: ");
	if(conf & 0x80)
		printf("2\n");
	else{
		if(conf & 0x40)
			printf("1\n");
		else
			printf("0\n");
	}

	if((conf & 0x40) & (conf & 0x80))
		return 1;
	if(!(conf & 0x10) & !(conf & 0x20))
		return 1;

	return 0;
}

int timer_test_time_base(unsigned long freq) {
	
	if(timer_set_frequency(0, freq) != 0)
		return 1;
	return 0;
}

int timer_test_int(unsigned long time) {
	

	timer_test_time_base(frequencianormal);

	int contador2 = 0;
	int r;
	char irq_set = BIT(hook);
	int ipc_status;
	message msg;


	if (timer_subscribe_int() == -1)
		return 1;

	while(contador < time) {
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_set) {
					contador2++;
					if ((contador2/frequencianormal) == 1)
					{
						printf("%lu Seconds\n", contador+1);
						contador2 = 0;
						timer_int_handler();
					}
				}
				break;
			default:
				break;
			}
		}
	}

	if(timer_unsubscribe_int() == -1)
		return 1;

	return 0;
}

int timer_test_config(unsigned char timer) {
	if (timer < 0 || timer > 2)
		return 1;
	
	unsigned char st;
	if(timer_get_conf(timer, &st)==0 && timer_display_conf(st)==0)
		return 0;
	else
		return 1;
}
