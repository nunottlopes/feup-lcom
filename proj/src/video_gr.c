#include <minix/syslib.h>
#include <minix/drivers.h>
#include <stdint.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "vbe.h"
#include "i8254.h"
#include "read_xpm.h"
#include "sprite.h"


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

	h_res = vmit.XResolution;
	v_res = vmit.YResolution;
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

int draw_time_score_bar(){

	short y=0;
	short x=0;

	unsigned short xtemporario = x;
	unsigned short xmax = 1024, ymax = 30;

	while(y <= ymax){
		while(xtemporario <= xmax){
			paint_pixel(xtemporario,y,23);
			xtemporario++;
		}
		xtemporario = x;
		y++;
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

int draw_number(int number, int x, int y){
	switch(number){
	case 0:
		draw_sprite(getZero_sprite(),x,y);
		break;
	case 1:
		draw_sprite(getOne_sprite(),x,y);
		break;
	case 2:
		draw_sprite(getTwo_sprite(),x,y);
		break;
	case 3:
		draw_sprite(getThree_sprite(),x,y);
		break;
	case 4:
		draw_sprite(getFour_sprite(),x,y);
		break;
	case 5:
		draw_sprite(getFive_sprite(),x,y);
		break;
	case 6:
		draw_sprite(getSix_sprite(),x,y);
		break;
	case 7:
		draw_sprite(getSeven_sprite(),x,y);
		break;
	case 8:
		draw_sprite(getEight_sprite(),x,y);
		break;
	case 9:
		draw_sprite(getNine_sprite(),x,y);
		break;
	}
	return 0;
}

int draw_bullets(int number){
	switch(number){
	case 10:
		draw_sprite(getBullet_sprite(),145,7);
		draw_sprite(getBullet_sprite(),162,7);
		draw_sprite(getBullet_sprite(),179,7);
		draw_sprite(getBullet_sprite(),196,7);
		draw_sprite(getBullet_sprite(),213,7);
		draw_sprite(getBullet_sprite(),230,7);
		draw_sprite(getBullet_sprite(),247,7);
		draw_sprite(getBullet_sprite(),264,7);
		draw_sprite(getBullet_sprite(),281,7);
		draw_sprite(getBullet_sprite(),298,7);
		break;
	case 9:
		draw_sprite(getBullet_sprite(),145,7);
		draw_sprite(getBullet_sprite(),162,7);
		draw_sprite(getBullet_sprite(),179,7);
		draw_sprite(getBullet_sprite(),196,7);
		draw_sprite(getBullet_sprite(),213,7);
		draw_sprite(getBullet_sprite(),230,7);
		draw_sprite(getBullet_sprite(),247,7);
		draw_sprite(getBullet_sprite(),264,7);
		draw_sprite(getBullet_sprite(),281,7);
		draw_sprite(getBullet_sprite2(),298,7);
		break;
	case 8:
		draw_sprite(getBullet_sprite(),145,7);
		draw_sprite(getBullet_sprite(),162,7);
		draw_sprite(getBullet_sprite(),179,7);
		draw_sprite(getBullet_sprite(),196,7);
		draw_sprite(getBullet_sprite(),213,7);
		draw_sprite(getBullet_sprite(),230,7);
		draw_sprite(getBullet_sprite(),247,7);
		draw_sprite(getBullet_sprite(),264,7);
		draw_sprite(getBullet_sprite2(),281,7);
		draw_sprite(getBullet_sprite2(),298,7);
		break;
	case 7:
		draw_sprite(getBullet_sprite(),145,7);
		draw_sprite(getBullet_sprite(),162,7);
		draw_sprite(getBullet_sprite(),179,7);
		draw_sprite(getBullet_sprite(),196,7);
		draw_sprite(getBullet_sprite(),213,7);
		draw_sprite(getBullet_sprite(),230,7);
		draw_sprite(getBullet_sprite(),247,7);
		draw_sprite(getBullet_sprite2(),264,7);
		draw_sprite(getBullet_sprite2(),281,7);
		draw_sprite(getBullet_sprite2(),298,7);
		break;
	case 6:
		draw_sprite(getBullet_sprite(),145,7);
		draw_sprite(getBullet_sprite(),162,7);
		draw_sprite(getBullet_sprite(),179,7);
		draw_sprite(getBullet_sprite(),196,7);
		draw_sprite(getBullet_sprite(),213,7);
		draw_sprite(getBullet_sprite(),230,7);
		draw_sprite(getBullet_sprite2(),247,7);
		draw_sprite(getBullet_sprite2(),264,7);
		draw_sprite(getBullet_sprite2(),281,7);
		draw_sprite(getBullet_sprite2(),298,7);
		break;
	case 5:
		draw_sprite(getBullet_sprite(),145,7);
		draw_sprite(getBullet_sprite(),162,7);
		draw_sprite(getBullet_sprite(),179,7);
		draw_sprite(getBullet_sprite(),196,7);
		draw_sprite(getBullet_sprite(),213,7);
		draw_sprite(getBullet_sprite2(),230,7);
		draw_sprite(getBullet_sprite2(),247,7);
		draw_sprite(getBullet_sprite2(),264,7);
		draw_sprite(getBullet_sprite2(),281,7);
		draw_sprite(getBullet_sprite2(),298,7);
		break;
	case 4:
		draw_sprite(getBullet_sprite(),145,7);
		draw_sprite(getBullet_sprite(),162,7);
		draw_sprite(getBullet_sprite(),179,7);
		draw_sprite(getBullet_sprite(),196,7);
		draw_sprite(getBullet_sprite2(),213,7);
		draw_sprite(getBullet_sprite2(),230,7);
		draw_sprite(getBullet_sprite2(),247,7);
		draw_sprite(getBullet_sprite2(),264,7);
		draw_sprite(getBullet_sprite2(),281,7);
		draw_sprite(getBullet_sprite2(),298,7);
		break;
	case 3:
		draw_sprite(getBullet_sprite(),145,7);
		draw_sprite(getBullet_sprite(),162,7);
		draw_sprite(getBullet_sprite(),179,7);
		draw_sprite(getBullet_sprite2(),196,7);
		draw_sprite(getBullet_sprite2(),213,7);
		draw_sprite(getBullet_sprite2(),230,7);
		draw_sprite(getBullet_sprite2(),247,7);
		draw_sprite(getBullet_sprite2(),264,7);
		draw_sprite(getBullet_sprite2(),281,7);
		draw_sprite(getBullet_sprite2(),298,7);
		break;
	case 2:
		draw_sprite(getBullet_sprite(),145,7);
		draw_sprite(getBullet_sprite(),162,7);
		draw_sprite(getBullet_sprite2(),179,7);
		draw_sprite(getBullet_sprite2(),196,7);
		draw_sprite(getBullet_sprite2(),213,7);
		draw_sprite(getBullet_sprite2(),230,7);
		draw_sprite(getBullet_sprite2(),247,7);
		draw_sprite(getBullet_sprite2(),264,7);
		draw_sprite(getBullet_sprite2(),281,7);
		draw_sprite(getBullet_sprite2(),298,7);
		break;
	case 1:
		draw_sprite(getBullet_sprite(),145,7);
		draw_sprite(getBullet_sprite2(),162,7);
		draw_sprite(getBullet_sprite2(),179,7);
		draw_sprite(getBullet_sprite2(),196,7);
		draw_sprite(getBullet_sprite2(),213,7);
		draw_sprite(getBullet_sprite2(),230,7);
		draw_sprite(getBullet_sprite2(),247,7);
		draw_sprite(getBullet_sprite2(),264,7);
		draw_sprite(getBullet_sprite2(),281,7);
		draw_sprite(getBullet_sprite2(),298,7);
		break;
	case 0:
		draw_sprite(getBullet_sprite2(),145,7);
		draw_sprite(getBullet_sprite2(),162,7);
		draw_sprite(getBullet_sprite2(),179,7);
		draw_sprite(getBullet_sprite2(),196,7);
		draw_sprite(getBullet_sprite2(),213,7);
		draw_sprite(getBullet_sprite2(),230,7);
		draw_sprite(getBullet_sprite2(),247,7);
		draw_sprite(getBullet_sprite2(),264,7);
		draw_sprite(getBullet_sprite2(),281,7);
		draw_sprite(getBullet_sprite2(),298,7);
		break;
	}
	return 0;
}

int draw_lives(int number){
	switch(number){
	case 5:
		draw_sprite(getHeart_sprite(),429,7);
		draw_sprite(getHeart_sprite(),446,7);
		draw_sprite(getHeart_sprite(),463,7);
		draw_sprite(getHeart_sprite(),480,7);
		draw_sprite(getHeart_sprite(),497,7);
		break;
	case 4:
		draw_sprite(getHeart_sprite(),429,7);
		draw_sprite(getHeart_sprite(),446,7);
		draw_sprite(getHeart_sprite(),463,7);
		draw_sprite(getHeart_sprite(),480,7);
		draw_sprite(getHeart_sprite2(),497,7);
		break;
	case 3:
		draw_sprite(getHeart_sprite(),429,7);
		draw_sprite(getHeart_sprite(),446,7);
		draw_sprite(getHeart_sprite(),463,7);
		draw_sprite(getHeart_sprite2(),480,7);
		draw_sprite(getHeart_sprite2(),497,7);
		break;
	case 2:
		draw_sprite(getHeart_sprite(),429,7);
		draw_sprite(getHeart_sprite(),446,7);
		draw_sprite(getHeart_sprite2(),463,7);
		draw_sprite(getHeart_sprite2(),480,7);
		draw_sprite(getHeart_sprite2(),497,7);
		break;
	case 1:
		draw_sprite(getHeart_sprite(),429,7);
		draw_sprite(getHeart_sprite2(),446,7);
		draw_sprite(getHeart_sprite2(),463,7);
		draw_sprite(getHeart_sprite2(),480,7);
		draw_sprite(getHeart_sprite2(),497,7);
		break;
	case 0:
		draw_sprite(getHeart_sprite2(),429,7);
		draw_sprite(getHeart_sprite2(),446,7);
		draw_sprite(getHeart_sprite2(),463,7);
		draw_sprite(getHeart_sprite2(),480,7);
		draw_sprite(getHeart_sprite2(),497,7);
		break;
	}

	return 0;
}

int draw_scope(int x, int y){
	int i, n, m;
	for(i = 7; i < 18; i++){
		paint_pixel(x+i,y,63);
		paint_pixel(x+i,y+24,63);
		paint_pixel(x,y+i,63);
		paint_pixel(x+24,y+i,63);
	}
	for(i = 0; i < 24; i++){
		paint_pixel(x+12,y+i,63);
		paint_pixel(x+i,y+12,63);
	}
	n = 6, m = 18;
	for(i = 1; i < 7; i++){
		paint_pixel(x+n,y+i,63);
		n--;
		paint_pixel(x+m,y+i,63);
		m++;
	}
	n = 1, m = 23;
	for(i = 18; i < 24; i++){
		paint_pixel(x+n,y+i,63);
		n++;
		paint_pixel(x+m,y+i,63);
		m--;
	}
	return 0;
}

int clean_scope(int x, int y){
	int i, n, m;
	for(i = 7; i < 18; i++){
		paint_pixel(x+i,y,0);
		paint_pixel(x+i,y+24,0);
		paint_pixel(x,y+i,0);
		paint_pixel(x+24,y+i,0);
	}
	for(i = 0; i < 24; i++){
		paint_pixel(x+12,y+i,0);
		paint_pixel(x+i,y+12,0);
	}
	n = 6, m = 18;
	for(i = 1; i < 7; i++){
		paint_pixel(x+n,y+i,0);
		n--;
		paint_pixel(x+m,y+i,0);
		m++;
	}
	n = 1, m = 23;
	for(i = 18; i < 24; i++){
		paint_pixel(x+n,y+i,0);
		n++;
		paint_pixel(x+m,y+i,0);
		m--;
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












