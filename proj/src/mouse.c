#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>

#include "mouse.h"
#include "i8042.h"

static int mousehook = 0x02;
mouse rato;
int mouse_counter = 0;


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

int mouse_game_handler(int *mouse_current_x, int *mouse_current_y){

	static unsigned char packet[3];
	unsigned long stat,type;
	sys_inb(STAT_REG, &stat);
	if(stat & OBF){
		if(sys_inb(OUT_BUF, &type) != OK)
			return 1;
	}
	packet[mouse_counter] = type;
	if ((packet[0] & BIT(3)) != 0){
		if(mouse_counter != 2){
			mouse_counter++;
			return 0;
		}
		else{
			rato.LB = packet[0]&BIT(0);
			if(packet[0]&BIT(4)){ //if x is negative
				signed char x = packet[1];
				rato.x = x;
				if (*mouse_current_x + rato.x <= 0){
					*mouse_current_x = 0;
				}
				else
					*mouse_current_x += rato.x;
			}
			else{
				rato.x = packet[1];
				if (*mouse_current_x + rato.x + 25 >= 1024){
					*mouse_current_x = 1024 - 25;
				}
				else
					*mouse_current_x += rato.x;
			}
			if(packet[0]&BIT(5)){ //if y is negative
				signed char y = packet[2];
				rato.y = y;
				if (*mouse_current_y - rato.y + 25 >= 768){
					*mouse_current_y = 768 - 25;
				}
				else
					*mouse_current_y -= rato.y;
			}
			else{
				rato.y = packet[2];
				if (*mouse_current_y - rato.y <= 30){
					*mouse_current_y = 31;
				}
				else
					*mouse_current_y -= rato.y;
			}
			mouse_counter = 0;
			return 2;
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
