#ifndef _GAME_H_
#define _GAME_H_


int open_game();

int mouse_game_handler();

int score_handler();

int time_handler();

int bullets_handler();

int lives_handler();

int writeHighscoresFile();

int readHighscoresFile();

int highscore_handler();

int draw_highscores();

#endif /* _GAME_H */
