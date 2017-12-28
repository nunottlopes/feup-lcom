#ifndef __SPRITE_H
#define __SPRITE_H


typedef struct {
	int width, height; // dimensions
	char *map; 	// the pixmap
} Sprite;

Sprite * create_sprite(char*pic[]);

void destroy_sprite(Sprite *sp);

int draw_sprite(Sprite* sp, int x, int y);


#endif
