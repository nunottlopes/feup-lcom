#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/driver.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

#include "sprite.h"
#include "read_xpm.h"
#include "video_gr.h"
#include "pixmap.h"

static Sprite* title = NULL;
static Sprite* playbutton = NULL;
static Sprite* playbutton2 = NULL;
static Sprite* highscoretitle = NULL;
static Sprite* highscoresbutton = NULL;
static Sprite* highscoresbutton2 = NULL;
static Sprite* quitbutton = NULL;
static Sprite* quitbutton2 = NULL;
static Sprite* time_bar = NULL;
static Sprite* score_bar = NULL;
static Sprite* bullettext_bar = NULL;
static Sprite* lives_bar = NULL;
static Sprite* monster_sprite = NULL;
static Sprite* monster_sprite2 = NULL;
static Sprite* gameover_sprite = NULL;
static Sprite* twodots_sprite = NULL;
static Sprite* zero_sprite = NULL;
static Sprite* one_sprite = NULL;
static Sprite* two_sprite = NULL;
static Sprite* three_sprite = NULL;
static Sprite* four_sprite = NULL;
static Sprite* five_sprite = NULL;
static Sprite* six_sprite = NULL;
static Sprite* seven_sprite = NULL;
static Sprite* eight_sprite = NULL;
static Sprite* nine_sprite = NULL;
static Sprite* bullet_sprite = NULL;
static Sprite* bullet_sprite2 = NULL;
static Sprite* heart_sprite = NULL;
static Sprite* heart_sprite2 = NULL;
static Sprite* bar_sprite = NULL;

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

void createSprites(){
	title = create_sprite(titulo);
	playbutton = create_sprite(play);
	playbutton2 = create_sprite(play2);
	highscoretitle = create_sprite(highscores3);
	highscoresbutton = create_sprite(highscores);
	highscoresbutton2 = create_sprite(highscores2);
	quitbutton = create_sprite(quit);
	quitbutton2 = create_sprite(quit2);
	time_bar = create_sprite(time);
	score_bar = create_sprite(score);
	bullettext_bar = create_sprite(bullettext);
	lives_bar = create_sprite(lives);
	monster_sprite = create_sprite(monster);
	monster_sprite2 = create_sprite(monster2);
	gameover_sprite = create_sprite(gameover);
	twodots_sprite = create_sprite(twodots);
	zero_sprite = create_sprite(zero);
	one_sprite = create_sprite(one);
	two_sprite = create_sprite(two);
	three_sprite = create_sprite(three);
	four_sprite = create_sprite(four);
	five_sprite = create_sprite(five);
	six_sprite = create_sprite(six);
	seven_sprite = create_sprite(seven);
	eight_sprite = create_sprite(eight);
	nine_sprite = create_sprite(nine);
	bullet_sprite = create_sprite(bullet);
	bullet_sprite2 = create_sprite(bullet2);
	heart_sprite = create_sprite(heart);
	heart_sprite2 = create_sprite(heart2);
	bar_sprite = create_sprite(bar);
}

void destroySprites(){
	destroy_sprite(title);
	destroy_sprite(playbutton);
	destroy_sprite(playbutton2);
	destroy_sprite(highscoretitle);
	destroy_sprite(highscoresbutton);
	destroy_sprite(highscoresbutton2);
	destroy_sprite(quitbutton);
	destroy_sprite(quitbutton2);
	destroy_sprite(time_bar);
	destroy_sprite(score_bar);
	destroy_sprite(bullettext_bar);
	destroy_sprite(lives_bar);
	destroy_sprite(monster_sprite);
	destroy_sprite(monster_sprite2);
	destroy_sprite(gameover_sprite);
	destroy_sprite(twodots_sprite);
	destroy_sprite(zero_sprite);
	destroy_sprite(one_sprite);
	destroy_sprite(two_sprite);
	destroy_sprite(three_sprite);
	destroy_sprite(four_sprite);
	destroy_sprite(five_sprite);
	destroy_sprite(six_sprite);
	destroy_sprite(seven_sprite);
	destroy_sprite(eight_sprite);
	destroy_sprite(nine_sprite);
	destroy_sprite(bullet_sprite);
	destroy_sprite(bullet_sprite2);
	destroy_sprite(heart_sprite);
	destroy_sprite(heart_sprite2);
	destroy_sprite(bar_sprite);
}

Sprite* getTitle(){
	return title;
}
Sprite* getPlaybutton(){
	return playbutton;
}
Sprite* getPlaybutton2(){
	return playbutton2;
}
Sprite* getHighscorestitle(){
	return highscoretitle;
}
Sprite* getHighscoresbutton(){
	return highscoresbutton;
}
Sprite* getHighscoresbutton2(){
	return highscoresbutton2;
}
Sprite* getQuitbutton(){
	return quitbutton;
}
Sprite* getQuitbutton2(){
	return quitbutton2;
}
Sprite* getTime_bar(){
	return time_bar;
}
Sprite* getScore_bar(){
	return score_bar;
}
Sprite* getBullettext_bar(){
	return bullettext_bar;
}
Sprite* getLives_bar(){
	return lives_bar;
}
Sprite* getMonster_sprite(){
	return monster_sprite;
}
Sprite* getMonster_sprite2(){
	return monster_sprite2;
}
Sprite* getGameover_sprite(){
	return gameover_sprite;
}
Sprite* getTwodots_sprite(){
	return twodots_sprite;
}
Sprite* getZero_sprite(){
	return zero_sprite;
}
Sprite* getOne_sprite(){
	return one_sprite;
}
Sprite* getTwo_sprite(){
	return two_sprite;
}
Sprite* getThree_sprite(){
	return three_sprite;
}
Sprite* getFour_sprite(){
	return four_sprite;
}
Sprite* getFive_sprite(){
	return five_sprite;
}
Sprite* getSix_sprite(){
	return six_sprite;
}
Sprite* getSeven_sprite(){
	return seven_sprite;
}
Sprite* getEight_sprite(){
	return eight_sprite;
}
Sprite* getNine_sprite(){
	return nine_sprite;
}
Sprite* getBullet_sprite(){
	return bullet_sprite;
}
Sprite* getBullet_sprite2(){
	return bullet_sprite2;
}
Sprite* getHeart_sprite(){
	return heart_sprite;
}
Sprite* getHeart_sprite2(){
	return heart_sprite2;
}
Sprite* getBar_sprite(){
	return bar_sprite;
}

