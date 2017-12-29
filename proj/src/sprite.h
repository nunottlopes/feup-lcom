#ifndef __SPRITE_H
#define __SPRITE_H


typedef struct {
	int width, height; // dimensions
	char *map; 	// the pixmap
} Sprite;

Sprite * create_sprite(char*pic[]);

void destroy_sprite(Sprite *sp);

int draw_sprite(Sprite* sp, int x, int y);

void createSprites();

void destroySprites();

Sprite* getTitle();
Sprite* getPlaybutton();
Sprite* getPlaybutton2();
Sprite* getHighscorestitle();
Sprite* getHighscoresbutton();
Sprite* getHighscoresbutton2();
Sprite* getQuitbutton();
Sprite* getQuitbutton2();
Sprite* getTime_bar();
Sprite* getScore_bar();
Sprite* getBullettext_bar();
Sprite* getLives_bar();
Sprite* getMonster_sprite();
Sprite* getMonster_sprite2();
Sprite* getGameover_sprite();
Sprite* getTwodots_sprite();
Sprite* getZero_sprite();
Sprite* getOne_sprite();
Sprite* getTwo_sprite();
Sprite* getThree_sprite();
Sprite* getFour_sprite();
Sprite* getFive_sprite();
Sprite* getSix_sprite();
Sprite* getSeven_sprite();
Sprite* getEight_sprite();
Sprite* getNine_sprite();
Sprite* getBullet_sprite();
Sprite* getBullet_sprite2();
Sprite* getHeart_sprite();
Sprite* getHeart_sprite2();
Sprite* getBar_sprite();

#endif
