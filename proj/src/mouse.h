#ifndef __MOUSE_H
#define __MOUSE_H


//Struct to control the mouse
typedef struct
{
	int x; //x_position
	int y; //y_position
	int LB;	//left button
}mouse;

int mouse_subscribe_int();

int mouse_unsubscribe_int();

int mouse_game_handler(int *mouse_current_x, int *mouse_current_y);

int start_mouse();

int end_mouse();

int send_command(long codigo);


#endif /* __MOUSE_H */
