#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#include "mouse.h"
#include "i8042.h"

static int mousehook = 0x02;
int counter = 0;

int mouse_subscribe_int(){
	int mousehooktemporario = mousehook;

	if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, & mousehook) == OK)
		if (sys_irqenable(& mousehook) == OK)
			return BIT(mousehooktemporario);

	return -1;
}

int mouse_unsubscribe_int(){
	if (sys_irqdisable(& mousehook) == OK)
		if (sys_irqrmpolicy(& mousehook) == OK)
			return 0;

	return -1;
}

int mouse_printf(){

	unsigned long stat,type;
	sys_inb(STAT_REG, &stat);
	if(stat & OBF){
		if(sys_inb(OUT_BUF, &type) != OK)
			return 1;
	}

	static unsigned char packet[3];

	packet[counter] = type;

	if ((packet[0] & BIT(3)) != 0){
		if(counter != 2){
			counter++;
			return 0;
		}
		else{
			printf("\nB1=0x%X  B2=0x%X  B3=0x%X", packet[0], packet[1], packet[2]);
			printf("  LB=%u  MB=%u  RB=%u", packet[0]&BIT(0), (packet[0]&BIT(2)) >> 2, (packet[0]&BIT(1)) >> 1);
			printf("  XOV=%u  YOV=%u", packet[0]&BIT(6), packet[0]&BIT(7));

			if(packet[0]&BIT(4)){ //check if x is negative
				signed char x = packet[1];
				printf("  X=%d", x);
			}
			else
				printf("  X=%d", packet[1]);

			if(packet[0]&BIT(5)){ //check if y is negative
				signed char y = packet[2];
				printf("  Y=%d", y);
			}
			else
				printf("  Y=%d", packet[2]);

			counter = 0;
			return 1;
	 	}
	}
	return 0;
}

int start_mouse(){
	send_command(SET_STREAM_MODE);
	send_command(E_DATA_REPORT);
	return 0;
}

int end_mouse(){
	send_command(D_DATA_REPORT);
	send_command(SET_STREAM_MODE);
	return 0;
}

int mouse_printf_gesture(short length){

	unsigned long stat,type;
	sys_inb(STAT_REG, &stat);
	if(stat & OBF){
		if(sys_inb(OUT_BUF, &type) != OK)
			return -1;
	}

	static unsigned char packet[3];

	int horizontal_move = 0, vertical_move = 0;

	packet[counter] = type;

	if ((packet[0] & BIT(3)) != 0){
		if(counter != 2){
			counter++;
			return 0;
		}
		else{
			printf("\nB1=0x%X  B2=0x%X  B3=0x%X", packet[0], packet[1], packet[2]);
			printf("  LB=%u  MB=%u  RB=%u", packet[0]&BIT(0), (packet[0]&BIT(2)) >> 2, (packet[0]&BIT(1)) >> 1);
			printf("  XOV=%u  YOV=%u", packet[0]&BIT(6), packet[0]&BIT(7));

			if(packet[0]&BIT(4)){ //check if x is negative
				signed char x = packet[1];
				printf("  X=%d", x);
				horizontal_move = - packet[1];
			}
			else{
				printf("  X=%d", packet[1]);
				horizontal_move = packet[1];
			}

			if(packet[0]&BIT(5)){ //check if y is negative
				signed char y = packet[2];
				printf("  Y=%d", y);
				vertical_move = - packet[2];
			}
			else{
				printf("  Y=%d", packet[2]);
				vertical_move = packet[2];
			}

			counter = 0;

			if (((packet[0]&BIT(1)) >> 1) == 1){ //right button pressed
				if(vertical_move <= 0)
					return 0;
				if(horizontal_move < 0){
					if(horizontal_move <= (- length))
						return 1;
					return 0;
				}
				else
					if(horizontal_move >= length)
						return 1;
				return 0;
			}
		}
	}
	return 0;
}


int send_command(long codigo){

	unsigned long byte, stat;
	int i = 5;

	sys_inb(STAT_REG, &stat);

	while(i > 0){
		if((stat & IBF) ==0){
			sys_outb(STAT_REG, 0xD4); //Write Byte to Mouse

			break;
		}
		tickdelay(DELAY_US);
		i--;
	}

	sys_inb(STAT_REG, &stat);

	while(i > 0){
		sys_outb(OUT_BUF, codigo);
		sys_inb(OUT_BUF, &byte);

		if(byte == 0xFA)
			return 0;

		tickdelay(DELAY_US);
		i--;
	}

	return 0;
}
