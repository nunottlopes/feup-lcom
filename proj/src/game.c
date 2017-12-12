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
	GAME_ON,	//game running
	GAME_OVER	//game ended
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
int mouseposition = 0;
int mouse_counter = 0;
int mouse_current_x = 512; //current x position of the mouse
int mouse_current_y = 384; //current y position of the mouse
int mouse_previous_x = 512; //previous x position to clear the last position of the mouse
int mouse_previous_y = 384; //previous y position to clear the last position of the mouse

int timer_counter_1 = 0;
int timer_counter_2 = 0;
int timer_counter_3 = 0;
int monster_current_x = 512; //current x position of the monster
int monster_current_y = 384; //current y position of the monster
int monster_previous_x = 512; //previous x position to clear the last position of the monster
int monster_previous_y = 384; //previous y position to clear the last position of the monster
int monster_speed = 120; //the monster starts with a delay of 2 seconds, every 5 seconds the speed increases

int score_points = 0; //score of the game, for every hit the player gets 10 points
int bullets = 10; //bullets in the weapon, every time space bar is pressed they are recharged
int game_time = 0; //the time the player plays the game (in seconds)

int player_lives = 5; //number of lives that the player has, if he misses one monster he loses one live
int hit = 0; //if 0 the monster was not yet hit, if 1 the monster was already hit


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

	//to make the monster first position random
	monster_current_x = rand() % 960;
	monster_current_y = (rand() % 674) + 30;

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
						pixmap = read_xpm(titulo, &width, &height);
						draw_pixmap(pixmap, 184, 100, width, height);
						pixmap = read_xpm(play2, &width, &height);
						draw_pixmap(pixmap, 362, 340, width, height);
						pixmap = read_xpm(quit, &width, &height);
						draw_pixmap(pixmap, 379, 480, width, height);
					}
					if(opcao == QUIT){
						pixmap = read_xpm(titulo, &width, &height);
						draw_pixmap(pixmap, 184, 100, width, height);
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
							draw_time_score_bar();
							pixmap = read_xpm(time, &width, &height);
							draw_pixmap(pixmap, 654, 5, width, height);
							pixmap = read_xpm(score, &width, &height);
							draw_pixmap(pixmap, 845, 5, width, height);
							pixmap = read_xpm(bullettext, &width, &height);
							draw_pixmap(pixmap, 10, 5, width, height);
							pixmap = read_xpm(lives, &width, &height);
							draw_pixmap(pixmap, 325, 5, width, height);
							score_handler();
							time_handler();
							bullets_handler();
							lives_handler();
						}
						if(opcao == QUIT) //exit game
							sair = -1;
					}
					if(type == ESPACO){
						if(opcao == GAME_ON){
						//to reload the weapon
							bullets = 10;
							bullets_handler();
						}
					}
				}

				//Handling the timer interrupts
				if (msg.NOTIFY_ARG & irq_set_timer) {
					if(opcao == GAME_ON){
						if(timer_counter_1/300){
							if(monster_speed > 30){
								monster_speed -= 5; //still need to check how fast i want the monster to start disappearing fast
							}
							timer_counter_1 = 0;
						}
						if(timer_counter_2/monster_speed){
							monster_previous_x = monster_current_x;
							monster_previous_y = monster_current_y;
							if(hit == 0){
								pixmap = read_xpm(monster2, &width, &height);
								draw_pixmap(pixmap, monster_previous_x, monster_previous_y, width, height);
								player_lives--;
							}
							draw_scope(mouse_current_x, mouse_current_y);
							monster_current_x = rand() % 960;
							monster_current_y = (rand() % 674) + 30;

							if(hit == 0){
								pixmap = read_xpm(monster, &width, &height);
								draw_pixmap(pixmap, monster_current_x, monster_current_y, width, height);
							}
							draw_scope(mouse_current_x, mouse_current_y);

							hit = 0;
							timer_counter_2 = 0;
						}
						if(timer_counter_3/60){
							game_time++;
							time_handler();
							timer_counter_3 = 0;
						}
						timer_counter_1++;
						timer_counter_2++;
						timer_counter_3++;
						lives_handler();
						if(player_lives == 0)
							opcao = GAME_OVER;
					}
					if(opcao == GAME_OVER){
						//cleans the screen, waits for 3 seconds and then exits the game
						//clean_screen();
						//sleep(3);
						sair = -1;
					}
				}

				//Handling the mouse interrupts
				if (msg.NOTIFY_ARG & irq_set_mouse) {
					if(opcao != GAME_ON){
						sys_inb(OUT_BUF, &mouse); //to clear the mouse buf when it isn't needed
					}
					if(opcao == GAME_ON){
						mouse_previous_x = mouse_current_x;
						mouse_previous_y = mouse_current_y;
						mouseposition = mouse_game_handler();
						if(mouseposition == 2){
							clean_scope(mouse_previous_x, mouse_previous_y);
							if(hit == 0){
								pixmap = read_xpm(monster, &width, &height);
								draw_pixmap(pixmap, monster_current_x, monster_current_y, width, height);
							}
						}
						if (hit == 0){
							pixmap = read_xpm(monster, &width, &height);
							draw_pixmap(pixmap, monster_current_x, monster_current_y, width, height);
						}
						draw_scope(mouse_current_x, mouse_current_y);

						//shooting (pressing mouse left button)
						if((rato.LB == 1) && (rato.x == 0) && (rato.y == 0) && (mouseposition == 2)){
							if(bullets > 0){
								//code to handle if the player hit the monster
								if(hit == 0){
									if((mouse_current_x > (monster_current_x - 12)) && (mouse_current_x < (monster_current_x + 52)) && (mouse_current_y > (monster_current_y - 12)) && (mouse_current_y < (monster_current_y + 52))){
										pixmap = read_xpm(monster2, &width, &height);
										draw_pixmap(pixmap, monster_current_x, monster_current_y, width, height);
										score_points += 10; //for each monster hit the score increases in 10 points
										hit = 1;
									}
								}
								bullets--;
							}
							score_handler();
							bullets_handler();
						}
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

	//AT THE END OF THE GAME if it's GAME OVER
	if(opcao == GAME_OVER){
		//cleans the screen, only showing the time, score,... accomplished and showing "GAME OVER", then waits for 3 seconds and exits the game
		clean_screen();
		draw_time_score_bar();
		pixmap = read_xpm(time, &width, &height);
		draw_pixmap(pixmap, 654, 5, width, height);
		pixmap = read_xpm(score, &width, &height);
		draw_pixmap(pixmap, 845, 5, width, height);
		pixmap = read_xpm(bullettext, &width, &height);
		draw_pixmap(pixmap, 10, 5, width, height);
		pixmap = read_xpm(lives, &width, &height);
		draw_pixmap(pixmap, 325, 5, width, height);
		score_handler();
		time_handler();
		bullets_handler();
		lives_handler();
		sleep(3);
	}

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
				if (mouse_current_x + rato.x <= 0)
				{
					mouse_current_x = 0;
				}
				else
					mouse_current_x += rato.x;
			}
			else{
				rato.x = packet[1];
				if (mouse_current_x + rato.x + 25 >= 1024)
				{
					mouse_current_x = 1024 - 25;
				}
				else
					mouse_current_x += rato.x;
			}

			if(packet[0]&BIT(5)){ //if y is negative
				signed char y = packet[2];
				rato.y = y;
				if (mouse_current_y - rato.y + 25 >= 768)
				{
					mouse_current_y = 768 - 25;
				}
				else
					mouse_current_y -= rato.y;
			}
			else{
				rato.y = packet[2];
				if (mouse_current_y - rato.y <= 30)
				{
					mouse_current_y = 31;
				}
				else
					mouse_current_y -= rato.y;
			}
			mouse_counter = 0;
			return 2;
	 	}
	}
	return 0;
}

int score_handler(){
	int milhares = score_points / 1000;
	int centenas = (score_points - milhares*1000)/100;
	int dezenas = (score_points - milhares*1000 - centenas*100)/10;
	int unidades = (score_points - milhares*1000 - centenas*100 - dezenas*10);
	draw_number(milhares,945,7);
	draw_number(centenas,964,7);
	draw_number(dezenas,983,7);
	draw_number(unidades,1002,7);
	return 0;
}

int time_handler(){
	int width, height;
	char *pixmap;
	int segundos = 0;
	int minutos = 0;
	pixmap = read_xpm(twodots, &width, &height);
	draw_pixmap(pixmap, 782, 8, width, height);
	minutos =  game_time / 60;
	segundos = game_time % 60;
	draw_number(minutos/10,744,7);
	draw_number(minutos%10,763,7);
	draw_number(segundos/10,791,7);
	draw_number(segundos%10,810,7);
	return 0;
}

int bullets_handler(){
	draw_bullets(bullets);
	return 0;
}

int lives_handler(){
	draw_lives(player_lives);
	return 0;
}
