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


int main(int argc, char **argv){

	sef_startup();

	vg_open(0x105); //1024x768

	int width, height;
	char *pixmap;

	int ipc_status, r;
	message msg;
	unsigned long type;

	int irq_set_kbd = kbd_subscribe_int();
	int irq_set_timer = timer_subscribe_int();

	if (irq_set_kbd == -1 || irq_set_timer == -1)
		return 1;

	int sair = 0;
	int opcao = 1; //FIRST MENU: 1- to press start, 2- to exit, 3- if the game already started
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
					if(opcao == 1){
						pixmap = read_xpm(play2, &width, &height);
						draw_pixmap(pixmap, 362, 340, width, height);
						pixmap = read_xpm(quit, &width, &height);
						draw_pixmap(pixmap, 379, 480, width, height);
					}
					if(opcao == 2){
						pixmap = read_xpm(play, &width, &height);
						draw_pixmap(pixmap, 362, 340, width, height);
						pixmap = read_xpm(quit2, &width, &height);
						draw_pixmap(pixmap, 379, 480, width, height);
					}
					if (type == ESC){
						sair = -1;
					}
					if(type == SETA_CIMA){
						if(opcao == 2)
							opcao = 1;
					}
					if(type == SETA_BAIXO){
						if(opcao == 1)
							opcao = 2;
					}
					if(type == ENTER){
						if(opcao == 1){
							opcao = 3;
							clean_screen();
							//start game
						}
						if(opcao == 2) //exit game
							sair = -1;
					}
					if(type == ESPACO){
						//to reload the weapon
					}
				}

				//Handling the timer interrupts
				if (msg.NOTIFY_ARG & irq_set_timer) {

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

	vg_exit();

	return 0;
}
