#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/driver.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

#include "i8042.h"
#include "i8254.h"
#include "kbd.h"
#include "lmlib.h"
#include "pixmap.h"
#include "read_xpm.h"
#include "sprite.h"
#include "timer.h"
#include "vbe.h"
#include "video_gr.h"
#include "mouse.h"
#include "game.h"

//State machine to control the game
enum menu{
	START,	//start button
	QUIT,	//quit button
	GAME_ON	//game running
};

//Struct to control the mouse
typedef struct
{
	int x; //x_position
	int y; //y_position
	int LB;	//left button
}mouse;

enum menu opcao = START; //The first position when the game starts is the start button

mouse rato;
int mouse_counter = 0;
int current_x = 512; //current x position of the mouse
int current_y = 384; //current y position of the mouse
int previous_x; //previous x position to clear the last position of the mouse
int previous_y; //previous y position to clear the last position of the mouse

int open_game(){

	vg_open(0x105); //1024x768

	int width, height;
	char *pixmap;

	int ipc_status, r;
	message msg;
	unsigned long type;

	int irq_set_kbd = kbd_subscribe_int();
	int irq_set_timer = timer_subscribe_int();
	int irq_set_mouse = mouse_subscribe_int();

	if (irq_set_kbd == -1 || irq_set_timer == -1 || irq_set_mouse == -1)
		return 1;

	start_mouse();
	unsigned long mouse; //to clear the mouse buf

	int sair = 0; //variable to exit the game

	while (sair != -1){
		if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:

				//Handling the keyboard interrupts
				if (msg.NOTIFY_ARG & irq_set_kbd) {
					type = kbd_test_scan_C();
					if(opcao == START){
						//pixmap = read_xpm(titulo, &width, &height);
						//draw_pixmap(pixmap, 10, 10, width, height);
						pixmap = read_xpm(play2, &width, &height);
						draw_pixmap(pixmap, 362, 340, width, height);
						pixmap = read_xpm(quit, &width, &height);
						draw_pixmap(pixmap, 379, 480, width, height);
					}
					if(opcao == QUIT){
						//pixmap = read_xpm(titulo, &width, &height);
						//draw_pixmap(pixmap, 10, 10, width, height);
						pixmap = read_xpm(play, &width, &height);
						draw_pixmap(pixmap, 362, 340, width, height);
						pixmap = read_xpm(quit2, &width, &height);
						draw_pixmap(pixmap, 379, 480, width, height);
					}
					if (type == ESC){
						sair = -1;
					}
					if(type == SETA_CIMA){
						if(opcao == QUIT)
							opcao = START;
					}
					if(type == SETA_BAIXO){
						if(opcao == START)
							opcao = QUIT;
					}
					if(type == ENTER){
						if(opcao == START){
							opcao = GAME_ON;
							clean_screen();
							//start game
						}
						if(opcao == QUIT) //exit game
							sair = -1;
					}
					if(type == ESPACO){
						if(opcao == GAME_ON){
						//to reload the weapon
						}
					}
				}

				//Handling the timer interrupts
				if (msg.NOTIFY_ARG & irq_set_timer) {
					if(opcao == GAME_ON){

					}
				}

				//Handling the mouse interrupts
				if (msg.NOTIFY_ARG & irq_set_mouse) {
					if(opcao != GAME_ON){
						sys_inb(OUT_BUF, &mouse); //to clear the mouse buf when it isn't needed
					}
					if(opcao == GAME_ON){
						previous_x = current_x;
						previous_y = current_y;
						if(mouse_game_handler() == 2){
							pixmap = read_xpm(scope2, &width, &height);
							draw_pixmap(pixmap, previous_x, previous_y, width, height);
						}
						pixmap = read_xpm(scope1, &width, &height);
						draw_pixmap(pixmap, current_x, current_y, width, height);
					}
				}

				break;
			default:
				break;
			}
		}
	}

	end_mouse();

	if(mouse_unsubscribe_int() == -1)
		return 1;
	if(kbd_unsubscribe_int() == -1)
		return 1;
	if(timer_unsubscribe_int() == -1)
		return 1;

	vg_exit();

	return 0;
}

int mouse_game_handler(){

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
				if (current_x + rato.x <= 0)
				{
					current_x = 0;
				}
				else
					current_x += rato.x;
			}
			else{
				rato.x = packet[1];
				if (current_x + rato.x + 25 >= 1024)
				{
					current_x = 1024 - 25;
				}
				else
					current_x += rato.x;
			}

			if(packet[0]&BIT(5)){ //if y is negative
				signed char y = packet[2];
				rato.y = y;
				if (current_y - rato.y + 25 >= 768)
				{
					current_y = 768 - 25;
				}
				else
					current_y -= rato.y;
			}
			else{
				rato.y = packet[2];
				if (current_y - rato.y <= 0)
				{
					current_y = 0;
				}
				else
					current_y -= rato.y;
			}
			mouse_counter = 0;
			return 2;
	 	}
	}
	return 0;
}


