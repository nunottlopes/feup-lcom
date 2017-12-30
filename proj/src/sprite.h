#ifndef __SPRITE_H
#define __SPRITE_H

/** @defgroup sprite sprite
 * @{
 *
 * Functions that draw all the xpm needed for the game
 */

/** @name  Sprite Struct*/
/**@{
 *
 * Struct that contains the information of one xpm
 */
typedef struct {
	int width, height; // dimensions
	char *map; 	// the pixmap
} Sprite;
/** @} end of Sprite Struct */

/**
 * @brief Create Sprite
 *
 * @param pic is xpm that will be read and created to a sprite
 *
 * @return Returns a pointer to the new sprite created
 */
Sprite * create_sprite(char*pic[]);

/**
 * @brief Destroy Sprite
 *
 * Clears the memory occupied by the sprite
 *
 * @param sp is a pointer to a sprite struct
 */
void destroy_sprite(Sprite *sp);

/**
 * @brief Draw Sprite
 *
 * @param sp is a pointer to a sprite struct
 * @param x position
 * @param y position
 *
 * @return Returns 0
 */
int draw_sprite(Sprite* sp, int x, int y);

/**
 * @brief Creates All Sprites
 */
void createSprites();

/**
 * @brief Destroys All Sprites
 */
void destroySprites();

/**
 * @brief Gets title sprite
 *
 * @return Returns pointer to title sprite
 */
Sprite* getTitle();

/**
 * @brief Gets playbutton sprite
 *
 * @return Returns pointer to playbutton sprite
 */
Sprite* getPlaybutton();

/**
 * @brief Gets playbutton2 sprite
 *
 * @return Returns pointer to playbutton2 sprite
 */
Sprite* getPlaybutton2();

/**
 * @brief Gets highscorestitle sprite
 *
 * @return Returns pointer to highscorestitle sprite
 */
Sprite* getHighscorestitle();

/**
 * @brief Gets highscoresbutton sprite
 *
 * @return Returns pointer to highscoresbutton sprite
 */
Sprite* getHighscoresbutton();

/**
 * @brief Gets highscoresbutton2 sprite
 *
 * @return Returns pointer to highscoresbutton2 sprite
 */
Sprite* getHighscoresbutton2();

/**
 * @brief Gets quitbutton sprite
 *
 * @return Returns pointer to quitbutton sprite
 */
Sprite* getQuitbutton();

/**
 * @brief Gets quitbutton2 sprite
 *
 * @return Returns pointer to quitbutton2 sprite
 */
Sprite* getQuitbutton2();

/**
 * @brief Gets time bar sprite
 *
 * @return Returns pointer to time bar sprite
 */
Sprite* getTime_bar();

/**
 * @brief Gets score bar sprite
 *
 * @return Returns pointer to score bar sprite
 */
Sprite* getScore_bar();

/**
 * @brief Gets bullettext bar sprite
 *
 * @return Returns pointer to bullettext bar sprite
 */
Sprite* getBullettext_bar();

/**
 * @brief Gets lives bar sprite
 *
 * @return Returns pointer to lives bar sprite
 */
Sprite* getLives_bar();

/**
 * @brief Gets monster sprite
 *
 * @return Returns pointer to monster sprite
 */
Sprite* getMonster_sprite();

/**
 * @brief Gets monster2 sprite
 *
 * @return Returns pointer to monster2 sprite
 */
Sprite* getMonster_sprite2();

/**
 * @brief Gets game over sprite
 *
 * @return Returns pointer to game over sprite
 */
Sprite* getGameover_sprite();

/**
 * @brief Gets two dots sprite
 *
 * @return Returns pointer to two dots sprite
 */
Sprite* getTwodots_sprite();

/**
 * @brief Gets number 0 sprite
 *
 * @return Returns pointer to number 0 sprite
 */
Sprite* getZero_sprite();

/**
 * @brief Gets number 1 sprite
 *
 * @return Returns pointer to number 1 sprite
 */
Sprite* getOne_sprite();

/**
 * @brief Gets number 2 sprite
 *
 * @return Returns pointer to number 2 sprite
 */
Sprite* getTwo_sprite();

/**
 * @brief Gets number 3 sprite
 *
 * @return Returns pointer to number 3 sprite
 */
Sprite* getThree_sprite();

/**
 * @brief Gets number 4 sprite
 *
 * @return Returns pointer to number 4 sprite
 */
Sprite* getFour_sprite();

/**
 * @brief Gets number 5 sprite
 *
 * @return Returns pointer to number 5 sprite
 */
Sprite* getFive_sprite();

/**
 * @brief Gets number 6 sprite
 *
 * @return Returns pointer to number 6 sprite
 */
Sprite* getSix_sprite();

/**
 * @brief Gets number 7 sprite
 *
 * @return Returns pointer to number 7 sprite
 */
Sprite* getSeven_sprite();

/**
 * @brief Gets number 8 sprite
 *
 * @return Returns pointer to number 8 sprite
 */
Sprite* getEight_sprite();

/**
 * @brief Gets number 9 sprite
 *
 * @return Returns pointer to number 9 sprite
 */
Sprite* getNine_sprite();

/**
 * @brief Gets bullet sprite
 *
 * @return Returns pointer to bullet sprite
 */
Sprite* getBullet_sprite();

/**
 * @brief Gets bullet2 sprite
 *
 * @return Returns pointer to bullet2 sprite
 */
Sprite* getBullet_sprite2();

/**
 * @brief Gets heart sprite
 *
 * @return Returns pointer to heart sprite
 */
Sprite* getHeart_sprite();

/**
 * @brief Gets heart2 sprite
 *
 * @return Returns pointer to heart2 sprite
 */
Sprite* getHeart_sprite2();

/**
 * @brief Gets bar sprite
 *
 * @return Returns pointer to bar sprite
 */
Sprite* getBar_sprite();

#endif
