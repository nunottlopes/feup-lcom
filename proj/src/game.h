#ifndef _GAME_H_
#define _GAME_H_


int open_game();

int keyboard_handler(unsigned long type);

int mouse_handler();

int timer_handler();

int score_handler();

int time_handler();

int bullets_handler();

int lives_handler();


#endif /* _GAME_H */
