#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/driver.h>
#include <minix/com.h>

#include "test4.h"
#include "mouse.h"
#include "i8042.h"
#include "timer.h"
#include "i8254.h"

int mouse_test_packet(unsigned short cnt){

	int ipc_status;
	message msg;
	int r;

	int irq_set = mouse_subscribe_int();
	if (irq_set == -1)
		return 1;

	if(start_mouse() == -1)
		return 1;

	 while (cnt > 0){
		 if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			 printf("driver_receive failed with: %d", r);
			 continue;
		 }
		 if (is_ipc_notify(ipc_status)) {
			 switch (_ENDPOINT_P(msg.m_source)) {
			 case HARDWARE:
				 if (msg.NOTIFY_ARG & irq_set) {
					 if(mouse_printf() == 1)
						 cnt--;
				 }
				 break;
			 default:
				 break;
			 }
		 }
	 }

	 if(end_mouse() == -1)
		 return 1;

	 if(mouse_unsubscribe_int() == -1)
		 return 1;

	 //code that makes kbd work again
	 unsigned long type;
	 sys_inb(OUT_BUF,&type);

	 return 0;
}	

int mouse_test_async(unsigned short idle_time){

	int ipc_status;
	message msg;
	int r;

	int contador = 0, contador2 = 0;

	int irq_set = mouse_subscribe_int();
	int irq_set_t = timer_subscribe_int();

	if (irq_set == -1 || irq_set_t == -1)
		return 1;

	if(start_mouse() == -1)
		return 1;

	 while (contador < idle_time){
		 if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			 printf("driver_receive failed with: %d", r);
			 continue;
		 }
		 if (is_ipc_notify(ipc_status)) {
			 switch (_ENDPOINT_P(msg.m_source)) {
			 case HARDWARE:
				 if (msg.NOTIFY_ARG & irq_set_t) {
					 contador2++;
					 if ((contador2/60) == 1)
					 {
						 contador2 = 0;
						 contador++;
					 }
				 }
				 if (msg.NOTIFY_ARG & irq_set) {
					 mouse_printf();
					 contador = 0; //to reset the timer
				 }
				 break;
			 default:
				 break;
			 }
		 }
	 }

	 if(end_mouse() == -1)
		 return 1;

	 if(mouse_unsubscribe_int() == -1)
		 return 1;

	 if(timer_unsubscribe_int() == -1)
		 return 1;

	 //code that makes kbd work again
	 unsigned long type;
	 sys_inb(OUT_BUF,&type);

	 return 0;
}	

int mouse_test_remote(unsigned long period, unsigned short cnt){

	if(send_command(D_DATA_REPORT) == 1)
		return 1;

	if(send_command(SET_REMOTE_MODE) == 1)
		return 1;

	if(mouse_subscribe_int() == -1)
		return 1;

	while (cnt > 0){
		int i = 3;
		if(send_command(READ_DATA) == 1)
			return 1;
		while(i > 0){
			if(mouse_printf() == 1)
				cnt--;
			i--;
		}
		tickdelay(period);
	}

	if(mouse_unsubscribe_int() == -1)
		return 1;

	if(send_command(SET_STREAM_MODE) == 1)
		return 1;

	if(send_command(D_DATA_REPORT) == 1)
		return 1;

	//code that makes kbd work again
	unsigned long type;
	sys_inb(OUT_BUF,&type);

	return 0;

}	

int mouse_test_gesture(short length){

	int ipc_status;
	message msg;
	int r;
	int positive_slope = 0;

	int irq_set = mouse_subscribe_int();
	if (irq_set == -1)
		return 1;

	if(start_mouse() == -1)
		return 1;

	 while (positive_slope != 1){
		 if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			 printf("driver_receive failed with: %d", r);
			 continue;
		 }
		 if (is_ipc_notify(ipc_status)) {
			 switch (_ENDPOINT_P(msg.m_source)) {
			 case HARDWARE:
				 if (msg.NOTIFY_ARG & irq_set) {
					 positive_slope = mouse_printf_gesture(length);
				 }
				 break;
			 default:
				 break;
			 }
		 }
	 }

	 if(end_mouse() == -1)
		 return 1;

	 if(mouse_unsubscribe_int() == -1)
		 return 1;

	 //code that makes kbd work again
	 unsigned long type;
	 sys_inb(OUT_BUF,&type);

	 return 0;
}	
