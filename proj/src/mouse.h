#ifndef __MOUSE_H
#define __MOUSE_H


int mouse_subscribe_int();

int mouse_unsubscribe_int();

int mouse_printf();

int start_mouse();

int end_mouse();

int mouse_printf_gesture(short length);

int send_command(long codigo);

#endif /* __MOUSE_H */
