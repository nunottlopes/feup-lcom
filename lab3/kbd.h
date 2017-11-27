#ifndef __KBD_H
#define __KBD_H

int kbd_subscribe_int();

int kbd_unsubscribe_int();

unsigned long kbd_test_scan_C();

unsigned long kbd_choosing_C_or_ASM(unsigned short ass);

int kbd_print(unsigned long type, int flag);


#endif
