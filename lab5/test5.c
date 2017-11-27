#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/driver.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

#include "lmlib.h"
#include "vbe.h"
#include "video_gr.h"
#include "test5.h"
#include "video_test.h"
#include "kbd.h"
#include "i8042.h"
#include "read_xpm.h"
#include "timer.h"
#include "sprite.h"

int video_test_init(unsigned short mode, unsigned short delay) {


	if(vg_open(mode) == NULL) //open video mode
		return 1;

	sleep(delay);

	vg_exit(); //return to text mode

	return 0;
}

int video_test_square(unsigned short x, unsigned short y, unsigned short size, unsigned long color) {

	vg_open(0x105);

	draw_square(x,y,size,color);

	video_dump_fb(); //call immediately after drawing the square


	//code to handle the kbd

	int ipc_status, r, flag = 0;
	message msg;
	 unsigned long type;
	 int counter = 0;

	 int irq_set = kbd_subscribe_int();

	 if (irq_set == -1)
	 		return 1;


	 while (type != ESC){
		 if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			 printf("driver_receive failed with: %d", r);
			 continue;
		 }
		 if (is_ipc_notify(ipc_status)) {
			 switch (_ENDPOINT_P(msg.m_source)) {
			 case HARDWARE:
				 if (msg.NOTIFY_ARG & irq_set) {
					 type = kbd_test_scan_C();
					 flag = kbd_print(type,flag);
					 counter++;
				 }
				 break;
			 default:
				 break;
			 }
		 }
	 }


	 if(kbd_unsubscribe_int() == -1)
		 return 1;

	 vg_exit();

	return 0;
}

int video_test_line(unsigned short xi, unsigned short yi, 
		           unsigned short xf, unsigned short yf, unsigned long color) {

	vg_open(0x105);

	draw_line(xi,yi,xf,yf,color);

	video_dump_fb(); //call immediately after drawing the line


	//code to handle the kbd

	int ipc_status, r, flag = 0;
	 message msg;
	 unsigned long type;
	 int counter = 0;

	 int irq_set = kbd_subscribe_int();

	 if (irq_set == -1)
	 		return 1;


	 while (type != ESC){
		 if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			 printf("driver_receive failed with: %d", r);
			 continue;
		 }
		 if (is_ipc_notify(ipc_status)) {
			 switch (_ENDPOINT_P(msg.m_source)) {
			 case HARDWARE:
				 if (msg.NOTIFY_ARG & irq_set) {
					 type = kbd_test_scan_C();
					 flag = kbd_print(type,flag);
					 counter++;
				 }
				 break;
			 default:
				 break;
			 }
		 }
	 }

	 if(kbd_unsubscribe_int() == -1)
		 return 1;

	 vg_exit();

	return 0;
}

int test_xpm(char *xpm[], unsigned short xi, unsigned short yi) {

	vg_open(0x105);

	int width, height;
	char *pixmap;
	//get pixmap from XPM
	pixmap = read_xpm(xpm, &width, &height);

	draw_pixmap(pixmap, xi, yi, width, height);

	video_dump_fb(); //call immediately after drawing sprite


	//code to handle the kbd

	int ipc_status, r, flag = 0;
	message msg;
	unsigned long type;
	int counter = 0;

	int irq_set = kbd_subscribe_int();

	if (irq_set == -1)
		return 1;

	while (type != ESC){
		if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_set) {
					type = kbd_test_scan_C();
					flag = kbd_print(type,flag);
					counter++;
				}
				break;
			default:
				break;
			}
		}
	}

	if(kbd_unsubscribe_int() == -1)
		return 1;

	vg_exit();

	return 0;
}

int test_move(char *xpm[], unsigned short xi, unsigned short yi, unsigned short xf, unsigned short yf, short s, unsigned short f) {

	vg_open(0x105);


	int ipc_status, r, flag = 0;
	message msg;
	unsigned long type;


	int irq_set_kbd = kbd_subscribe_int();
	int irq_set_timer = timer_subscribe_int();

	if (irq_set_kbd == -1 || irq_set_timer == -1)
		return 1;

	Sprite * sprite;


	//If speed is positive it should be understood as the displacement in pixels between consecutive frames.
	//If the speed is negative it should be understood as the number of frames required for a displacement of one pixel.

	sprite = create_sprite(xpm, xi, yi, s, s);
	timer_set_frequency(0, f);

	int fim = 0;

	while ((type != ESC) && (fim == 0)){
		if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_set_kbd) {
					type = kbd_test_scan_C();
					flag = kbd_print(type,flag);
				}
				if (msg.NOTIFY_ARG & irq_set_timer) {

					clean_screen(); //screen all black
					draw_sprite(sprite);
					video_dump_fb(); //every time you complete rendering the XPM in the frame-buffer

					//You need only consider movements along either the horizontal or the vertical directions.
					if(s > 0){
						if((xi == xf) && (yi < yf)){
							yi += s;
							sprite->y += s;
						}
						if((yi == yf) && (xi < xf)){
							xi += s;
							sprite->x += s;
						}
					}
					if(s < 0){
						if((xi == xf) && (yi < yf)){
							yi -= s;
							sprite->y -= s;
						}
						if((yi == yf) && (xi < xf)){
							xi -= s;
							sprite->x -= s;
						}
					}
					if((xi == xf) && (yi == yf))
						fim = 1;
				}
				break;
			default:
				break;
			}
		}
	}

	destroy_sprite(sprite);

	if(kbd_unsubscribe_int() == -1)
		return 1;
	if(timer_unsubscribe_int() == -1)
		return 1;

	vg_exit();

	return 0;
}

int test_controller() {

	return 0;

}
