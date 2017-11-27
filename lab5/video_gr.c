#include <minix/syslib.h>
#include <minix/drivers.h>
#include <stdint.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "vbe.h"
#include "i8254.h"
#include "video_test.h"

/* Constants for VBE 0x105 mode */

/* The physical address may vary from VM to VM.
 * At one time it was 0xD0000000
 *  #define VRAM_PHYS_ADDR    0xD0000000 
 * Currently on lab B107 is 0xF0000000
 * Better run my version of lab5 as follows:
 *     service run `pwd`/lab5 -args "mode 0x105"
 */

/* Private global variables */

static char *video_mem;		/* Process (virtual) address to which VRAM is mapped */

static unsigned h_res;		/* Horizontal screen resolution in pixels */
static unsigned v_res;		/* Vertical screen resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

int vg_exit() {
  struct reg86u reg86;

  reg86.u.b.intno = 0x10; /* BIOS video services */

  reg86.u.b.ah = 0x00;    /* Set Video Mode function */
  reg86.u.b.al = 0x03;    /* 80x25 text mode*/

  if( sys_int86(&reg86) != OK ) {
      printf("\tvg_exit(): sys_int86() failed \n");
      return 1;
  } else
      return 0;
}

void * vg_open(unsigned short mode){

	struct reg86u reg86;

	reg86.u.b.intno = 0x10; /* BIOS video services */

	reg86.u.b.ah = 0x4F;   /* Invoking a VBE function */
	reg86.u.b.al = 0x02;   /* Set video mode */
	reg86.u.w.bx = (BIT(14) | mode);  //Bit 14 of the BX register should be set to use a linear frame buffer model, which facilitates access to VRAM.

	if( sys_int86(&reg86) != OK ) {
		panic("\tvg_exit(): sys_int86() failed \n");
		return NULL;
	}
	if(reg86.u.b.al != 0x4F){
		panic("VBE function called is not supported\n");
		return NULL;
	}
	if (reg86.u.b.ah == 0x01){
		panic("Function call failed\n");
		return NULL;
	}
	if (reg86.u.b.ah == 0x02){
		panic("Function is not supported in current HW configuration\n");
		return NULL;
	}
	if(reg86.u.b.ah == 0x03){
		panic("Function is invalid in current video mode\n");
		return NULL;
	}


	// Mapping VRAM in a Process's Address Space

	vbe_mode_info_t vmit;
	vbe_get_mode_info(mode,&vmit);


	if(mode == 0x101){
		h_res = 640;
		v_res = 480;
	}
	else if (mode == 0x103){
		h_res = 800;
		v_res = 600;
	}
	else if(mode == 0x105){
		h_res = 1024;
		v_res = 768;
	}
	else if(mode == 0x107){
		h_res = 1280;
		v_res = 1024;
	}

	bits_per_pixel = vmit.BitsPerPixel;
	unsigned bytes_per_pixel = bits_per_pixel / 8;

	int r;
	struct mem_range mr;
	unsigned int vram_base = vmit.PhysBasePtr;  /* VRAM's physical addresss */
	unsigned int vram_size = h_res * v_res * bytes_per_pixel;  /* VRAM's size, but you can use the frame-buffer size, instead */


	/* Allow memory mapping */

	mr.mr_base = (phys_bytes) vram_base;
	mr.mr_limit = mr.mr_base + vram_size;

	if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
		panic("sys_privctl (ADD_MEM) failed: %d\n", r);

	/* Map memory */

	video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

	if(video_mem == MAP_FAILED){
		panic("couldn't map video memory");
		//return NULL;
	}

	return video_mem;
}

int paint_pixel(unsigned short x, unsigned short y, unsigned long color){
	if (x > h_res || y > v_res)
		return 1;
	unsigned bytes_per_pixel = bits_per_pixel / 8;
	char *pointer;
	pointer = video_mem;
	pointer = pointer + (x + h_res * y) * bytes_per_pixel;
	*pointer = color;
	return 0;
}

int draw_square(unsigned short x, unsigned short y, unsigned short size, unsigned long color){

	unsigned centroh = h_res/2, centrov = v_res/2;
	y = centrov + y - size / 2;
	x = centroh + x - size / 2;

	unsigned short xtemporario = x;
	unsigned short xmax = x + size, ymax = y + size;


	while(y <= ymax){
		while(xtemporario <= xmax){
			paint_pixel(xtemporario,y,color);
			xtemporario++;
		}
		xtemporario = x;
		y++;
	}

	return 0;
}


int draw_line(unsigned short xi, unsigned short yi, unsigned short xf, unsigned short yf, unsigned long color){

	//Bresenham’s Line Drawing Algorithm

	int dx =  abs (xf - xi);
	int dy = -abs (yf - yi);
	int sx, sy;

	if(xi < xf)
		sx = 1;
	else
		sx = -1;

	if(yi < yf)
		sy = 1;
	else
		sy = -1;

	int erro = dx + dy;
	int e2;

	while (1){
		paint_pixel(xi,yi,color);
		if (xi == xf && yi == yf)
			break;
		e2 = 2 * erro;
		if (e2 >= dy) {
			erro += dy;
			xi += sx;
		}
		if (e2 <= dx) {
			erro += dx;
			yi += sy;
		}
	}
	return 0;
}


int draw_pixmap(char *pixmap, short x, short y, int width, int height){

	int width_counter = 0;
	int height_counter = 0;
	unsigned bytes_per_pixel = bits_per_pixel / 8;

	while(width_counter < width){
		while(height_counter < height){
			short new_x = x + width_counter;
			short new_y = y + height_counter;
			paint_pixel(new_x, new_y, *(pixmap +(height_counter * width + width_counter) * bytes_per_pixel));
			height_counter++;
		}
		height_counter = 0;
		width_counter++;
	}

	return 0;
}

int clean_screen(){

	//puts the screen all black

	int width_counter = 0;
	int height_counter = 0;

	while(width_counter < h_res){
		while(height_counter < v_res){
			paint_pixel(width_counter, height_counter, 0);
			height_counter++;
		}
		height_counter = 0;
		width_counter++;
	}
	return 0;
}












