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
#include "highscores.h"


//State machine to control the game
enum menu{
	START,		//start button
	HIGHSCORES,	//highscores button
	QUIT,		//quit button
	GAME_ON,	//game running
	GAME_OVER,	//game ended
	SCORES 		//show highscores
};

enum menu opcao = START; //The first position when the game starts is the start button

mouse rato; //initialize mouse struct
int mouseposition = 0; //mouse current status, helps drawing the scope
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


//Timer counters
int timer_counter_1 = 0; //counts 5 seconds and increases monster show/disappearance speed
int timer_counter_2 = 0; //counts the time the monster stays up in the screen before it disappears
int timer_counter_3 = 0; //counts the game time

int sair = 0; //variable to exit the game
unsigned long mouseinfo; //to clear the mouse buf when it's not needed


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

	//to make the monster first position random
	monster_current_x = rand() % 960;
	monster_current_y = (rand() % 674) + 30;

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
					keyboard_handler(type);
				}

				//Handling the timer interrupts
				if (msg.NOTIFY_ARG & irq_set_timer) {
					timer_handler();
				}

				//Handling the mouse interrupts
				if (msg.NOTIFY_ARG & irq_set_mouse) {
					mouse_handler();
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

int keyboard_handler(unsigned long type){
	//Handling state machine
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


	//Handling the keyboard button pressed
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
			readHighscoresFile();
			highscore_handler(score_points);
			writeHighscoresFile();
			timer_counter_1 = 0;
			timer_counter_2 = 0;
			timer_counter_3 = 0;
			monster_speed = 120;
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
		//reloads the weapon
			bullets = 10;
			bullets_handler();
		}
	}

	return 0;
}

int mouse_handler(){
	if(opcao != GAME_ON){
		sys_inb(OUT_BUF, &mouseinfo); //to clear the mouse buf when it isn't needed
	}
	if(opcao == GAME_ON){
		mouse_previous_x = mouse_current_x;
		mouse_previous_y = mouse_current_y;
		mouseposition = mouse_game_handler(&mouse_current_x, &mouse_current_y);
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
	return 0;
}

int timer_handler(){
	if(opcao == GAME_ON){
		if(timer_counter_1/300){
			if(monster_speed > 30){
				monster_speed -= 3;
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
