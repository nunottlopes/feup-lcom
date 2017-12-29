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
#include "read_xpm.h"
#include "sprite.h"
#include "timer.h"
#include "vbe.h"
#include "video_gr.h"
#include "mouse.h"
#include "game.h"
#include "rtc.h"

//State machine to control the game
enum menu{
	START,		//start button
	HIGHSCORES,	//highscores button
	QUIT,		//quit button
	GAME_ON,	//game running
	GAME_OVER,	//game ended
	SCORES 		//show highscores
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

//variables that store the three best highscores and also the time and date when they were accomplished
unsigned long score1, hours1, minutes1, day1, month1, year1;
unsigned long score2, hours2, minutes2, day2, month2, year2;
unsigned long score3, hours3, minutes3, day3, month3, year3;


int open_game(){

	vg_open(0x105); //1024x768

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

	int timer_counter_1 = 0;
	int timer_counter_2 = 0;
	int timer_counter_3 = 0;

	createSprites(); //creates all the xpm needed

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
						draw_sprite(getTitle(),184,75);
						draw_sprite(getPlaybutton2(),362,300);
						draw_sprite(getHighscoresbutton(),167,430);
						draw_sprite(getQuitbutton(),379,560);
					}
					if(opcao == HIGHSCORES){
						draw_sprite(getTitle(),184,75);
						draw_sprite(getPlaybutton(),362,300);
						draw_sprite(getHighscoresbutton2(),167,430);
						draw_sprite(getQuitbutton(),379,560);
					}
					if(opcao == QUIT){
						draw_sprite(getTitle(),184,75);
						draw_sprite(getPlaybutton(),362,300);
						draw_sprite(getHighscoresbutton(),167,430);
						draw_sprite(getQuitbutton2(),379,560);
					}
					if (type == ESC){
						if(opcao == SCORES){
							clean_screen();
							opcao = START;
							draw_sprite(getTitle(),184,75);
							draw_sprite(getPlaybutton2(),362,300);
							draw_sprite(getHighscoresbutton(),167,430);
							draw_sprite(getQuitbutton(),379,560);
						}
						else if(opcao == GAME_OVER){
							highscore_handler();
							writeHighscoresFile();
							timer_counter_1 = 0;
							timer_counter_2 = 0;
							timer_counter_3 = 0;
							score_points = 0;
							bullets = 10;
							game_time = 0;
							player_lives = 5;
							hit = 0;
							opcao = START;
							clean_screen();
							draw_sprite(getTitle(),184,75);
							draw_sprite(getPlaybutton2(),362,300);
							draw_sprite(getHighscoresbutton(),167,430);
							draw_sprite(getQuitbutton(),379,560);
						}
						else
							sair = -1;
					}
					if(type == SETA_CIMA){
						if(opcao == QUIT)
							opcao = HIGHSCORES;
						else if(opcao == HIGHSCORES)
							opcao = START;
					}
					if(type == SETA_BAIXO){
						if(opcao == START)
							opcao = HIGHSCORES;
						else if(opcao == HIGHSCORES)
							opcao = QUIT;
					}
					if(type == ENTER){
						if(opcao == START){
							opcao = GAME_ON;
							clean_screen();
							draw_time_score_bar();
							draw_sprite(getTime_bar(),654,5);
							draw_sprite(getScore_bar(),845,5);
							draw_sprite(getBullettext_bar(),10,5);
							draw_sprite(getLives_bar(),325,5);
							score_handler();
							time_handler();
							bullets_handler();
							lives_handler();
						}
						if(opcao == HIGHSCORES){
							opcao = SCORES;
							clean_screen();
							draw_sprite(getHighscorestitle(),167,75);
							readHighscoresFile();
							draw_highscores();
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
								monster_speed -= 5;
							}
							timer_counter_1 = 0;
						}
						if(timer_counter_2/monster_speed){
							monster_previous_x = monster_current_x;
							monster_previous_y = monster_current_y;
							if(hit == 0){
								draw_sprite(getMonster_sprite2(),monster_previous_x,monster_previous_y);
								player_lives--;
							}
							draw_scope(mouse_current_x, mouse_current_y);
							monster_current_x = rand() % 960;
							monster_current_y = (rand() % 674) + 30;

							if(hit == 0){
								draw_sprite(getMonster_sprite(),monster_current_x,monster_current_y);
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
						if(hit == 0){
							draw_sprite(getMonster_sprite(),monster_current_x,monster_current_y);
						}
						draw_scope(mouse_current_x, mouse_current_y);
						timer_counter_1++;
						timer_counter_2++;
						timer_counter_3++;
						lives_handler();
						if(player_lives == 0)
							opcao = GAME_OVER;
					}
					if(opcao == GAME_OVER){
						clean_scope(mouse_current_x, mouse_current_y);
						draw_sprite(getMonster_sprite2(),monster_current_x,monster_current_y);
						draw_sprite(getGameover_sprite(),165,300);
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
						}
						if (hit == 0){
							draw_sprite(getMonster_sprite(),monster_current_x,monster_current_y);
						}
						draw_scope(mouse_current_x, mouse_current_y);

						//shooting (pressing mouse left button)
						if((rato.LB == 1) && (rato.x == 0) && (rato.y == 0) && (mouseposition == 2)){
							if(bullets > 0){
								//code to handle if the player hit the monster
								if(hit == 0){
									if((mouse_current_x > (monster_current_x - 12)) && (mouse_current_x < (monster_current_x + 52)) && (mouse_current_y > (monster_current_y - 12)) && (mouse_current_y < (monster_current_y + 52))){
										draw_sprite(getMonster_sprite2(),monster_current_x,monster_current_y);
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

	destroySprites(); //frees all the memory used by the xpm

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
				if (mouse_current_x + rato.x <= 0){
					mouse_current_x = 0;
				}
				else
					mouse_current_x += rato.x;
			}
			else{
				rato.x = packet[1];
				if (mouse_current_x + rato.x + 25 >= 1024){
					mouse_current_x = 1024 - 25;
				}
				else
					mouse_current_x += rato.x;
			}
			if(packet[0]&BIT(5)){ //if y is negative
				signed char y = packet[2];
				rato.y = y;
				if (mouse_current_y - rato.y + 25 >= 768){
					mouse_current_y = 768 - 25;
				}
				else
					mouse_current_y -= rato.y;
			}
			else{
				rato.y = packet[2];
				if (mouse_current_y - rato.y <= 30){
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
	int segundos = 0;
	int minutos = 0;
	draw_sprite(getTwodots_sprite(),782,7);
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

int writeHighscoresFile(){
	FILE *fptr;
	fptr = fopen("/home/lcom/lcom//proj/src/highscores.txt", "w");
	fprintf(fptr,"%d\n",score1);
	fprintf(fptr,"%d\n",hours1);
	fprintf(fptr,"%d\n",minutes1);
	fprintf(fptr,"%d\n",day1);
	fprintf(fptr,"%d\n",month1);
	fprintf(fptr,"%d\n",year1);
	fprintf(fptr,"%d\n",score2);
	fprintf(fptr,"%d\n",hours2);
	fprintf(fptr,"%d\n",minutes2);
	fprintf(fptr,"%d\n",day2);
	fprintf(fptr,"%d\n",month2);
	fprintf(fptr,"%d\n",year2);
	fprintf(fptr,"%d\n",score3);
	fprintf(fptr,"%d\n",hours3);
	fprintf(fptr,"%d\n",minutes3);
	fprintf(fptr,"%d\n",day3);
	fprintf(fptr,"%d\n",month3);
	fprintf(fptr,"%d",year3);
	fclose(fptr);
	return 0;
}

int readHighscoresFile(){
	FILE *fptr;
	fptr = fopen("/home/lcom/lcom//proj/src/highscores.txt", "r");
	fscanf(fptr,"%d",&score1);
	fscanf(fptr,"%d",&hours1);
	fscanf(fptr,"%d",&minutes1);
	fscanf(fptr,"%d",&day1);
	fscanf(fptr,"%d",&month1);
	fscanf(fptr,"%d",&year1);
	fscanf(fptr,"%d",&score2);
	fscanf(fptr,"%d",&hours2);
	fscanf(fptr,"%d",&minutes2);
	fscanf(fptr,"%d",&day2);
	fscanf(fptr,"%d",&month2);
	fscanf(fptr,"%d",&year2);
	fscanf(fptr,"%d",&score3);
	fscanf(fptr,"%d",&hours3);
	fscanf(fptr,"%d",&minutes3);
	fscanf(fptr,"%d",&day3);
	fscanf(fptr,"%d",&month3);
	fscanf(fptr,"%d",&year3);
	fclose(fptr);
	return 0;
}

int highscore_handler(){
	if(score_points >= score1){
		score3 = score2;
		hours3 = hours2;
		minutes3 = minutes2;
		day3 = day2;
		month3 = month2;
		year3 = year2;
		score2 = score1;
		hours2 = hours1;
		minutes2 = minutes1;
		day2 = day1;
		month2 = month1;
		year2 = year1;
		score1 = score_points;
		rtc_get_date(&day1, &month1, &year1);
		rtc_get_time(&hours1, &minutes1);
		return 0;
	}
	if((score_points < score1) && (score_points >= score2)){
		score3 = score2;
		hours3 = hours2;
		minutes3 = minutes2;
		day3 = day2;
		month3 = month2;
		year3 = year2;
		score2 = score_points;
		rtc_get_date(&day2, &month2, &year2);
		rtc_get_time(&hours2, &minutes2);
		return 0;
	}
	if((score_points < score1) && (score_points < score2) && (score_points >= score3)){
		score3 = score_points;
		rtc_get_date(&day3, &month3, &year3);
		rtc_get_time(&hours3, &minutes3);
		return 0;
	}
	return 0;
}

int draw_highscores(){
	int milhares, centenas, dezenas, unidades;

	if(score1 != 0){
		dezenas = hours1/10;
		unidades = hours1 - dezenas*10;
		draw_number(dezenas,315,350);
		draw_number(unidades,333,350);
		draw_sprite(getTwodots_sprite(),351,350);
		dezenas = minutes1/10;
		unidades = minutes1 - dezenas*10;
		draw_number(dezenas,359,350);
		draw_number(unidades,377,350);
		dezenas = day1/10;
		unidades = day1 - dezenas*10;
		draw_number(dezenas,415,350);
		draw_number(unidades,433,350);
		draw_sprite(getBar_sprite(),451,350);
		dezenas = month1/10;
		unidades = month1 - dezenas*10;
		draw_number(dezenas,465,350);
		draw_number(unidades,483,350);
		draw_sprite(getBar_sprite(),501,350);
		dezenas = year1/10;
		unidades = year1 - dezenas*10;
		draw_number(dezenas,515,350);
		draw_number(unidades,533,350);
		milhares = score1 / 1000;
		centenas = (score1 - milhares*1000)/100;
		dezenas = (score1 - milhares*1000 - centenas*100)/10;
		unidades = (score1 - milhares*1000 - centenas*100 - dezenas*10);
		draw_number(milhares,632,350);
		draw_number(centenas,650,350);
		draw_number(dezenas,668,350);
		draw_number(unidades,686,350);
	}

	if(score2 != 0){
		dezenas = hours2/10;
		unidades = hours2 - dezenas*10;
		draw_number(dezenas,315,400);
		draw_number(unidades,333,400);
		draw_sprite(getTwodots_sprite(),351,400);
		dezenas = minutes2/10;
		unidades = minutes2 - dezenas*10;
		draw_number(dezenas,359,400);
		draw_number(unidades,377,400);
		dezenas = day2/10;
		unidades = day2 - dezenas*10;
		draw_number(dezenas,415,400);
		draw_number(unidades,433,400);
		draw_sprite(getBar_sprite(),451,400);
		dezenas = month2/10;
		unidades = month2 - dezenas*10;
		draw_number(dezenas,465,400);
		draw_number(unidades,483,400);
		draw_sprite(getBar_sprite(),501,400);
		dezenas = year2/10;
		unidades = year2 - dezenas*10;
		draw_number(dezenas,515,400);
		draw_number(unidades,533,400);
		milhares = score2 / 1000;
		centenas = (score2 - milhares*1000)/100;
		dezenas = (score2 - milhares*1000 - centenas*100)/10;
		unidades = (score2 - milhares*1000 - centenas*100 - dezenas*10);
		draw_number(milhares,632,400);
		draw_number(centenas,650,400);
		draw_number(dezenas,668,400);
		draw_number(unidades,686,400);
	}

	if(score3 != 0){
		dezenas = hours3/10;
		unidades = hours3 - dezenas*10;
		draw_number(dezenas,315,450);
		draw_number(unidades,333,450);
		draw_sprite(getTwodots_sprite(),351,450);
		dezenas = minutes3/10;
		unidades = minutes3 - dezenas*10;
		draw_number(dezenas,359,450);
		draw_number(unidades,377,450);
		dezenas = day3/10;
		unidades = day3 - dezenas*10;
		draw_number(dezenas,415,450);
		draw_number(unidades,433,450);
		draw_sprite(getBar_sprite(),451,450);
		dezenas = month3/10;
		unidades = month3 - dezenas*10;
		draw_number(dezenas,465,450);
		draw_number(unidades,483,450);
		draw_sprite(getBar_sprite(),501,450);
		dezenas = year3/10;
		unidades = year3 - dezenas*10;
		draw_number(dezenas,515,450);
		draw_number(unidades,533,450);
		milhares = score3 / 1000;
		centenas = (score3 - milhares*1000)/100;
		dezenas = (score3 - milhares*1000 - centenas*100)/10;
		unidades = (score3 - milhares*1000 - centenas*100 - dezenas*10);
		draw_number(milhares,632,450);
		draw_number(centenas,650,450);
		draw_number(dezenas,668,450);
		draw_number(unidades,686,450);
	}
	return 0;
}
