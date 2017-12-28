#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/driver.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

#include "sprite.h"
#include "read_xpm.h"
#include "video_gr.h"



Sprite * create_sprite(char*pic[]) {

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


int draw_sprite(Sprite* sp, int x, int y) {

	draw_pixmap(sp->map, x, y, sp->width, sp->height);
	return 0;
}

