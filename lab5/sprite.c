#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/driver.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

#include "sprite.h"
#include "read_xpm.h"
#include "video_gr.h"


Sprite * create_sprite(char*pic[], int x, int y, int xspeed, int yspeed) {

	//allocate space for the "object"
	Sprite*sp = (Sprite*) malloc ( sizeof(Sprite));
	if( sp == NULL )
		return NULL;

	// read the sprite pixmap
	sp->map = read_xpm(pic, &(sp->width), &(sp->height));
	if( sp->map == NULL ) {
		free(sp);
		return NULL;
	}

	sp->xspeed = xspeed;
	sp->yspeed = yspeed;
	sp->x = x;
	sp->y = y;

	return sp;
}

void destroy_sprite(Sprite *sp){
	if( sp == NULL )
		return;
	if ( sp ->map)
		free(sp->map);
	free(sp);
	sp = NULL;
}


/*Some useful non-visible functions*/
int draw_sprite(Sprite* sp) {

	draw_pixmap(sp->map, sp->x, sp->y, sp->width, sp->height);
	return 0;
}



