#ifndef _GAME_H_
#define _GAME_H_

/** @defgroup game game
 * @{
 *
 * Functions that create and handle the game
 */

/**
 * @brief Starts the game
 *
 * Subscribes all the devices interrupts and handles them. It's the game main function
 *
 * @return Return 0 upon success and 1 otherwise
 */
int open_game();

/**
 * @brief Handles keyboard interrupts
 *
 * @param type is the scancode from the OUT_BUF
 *
 * @return Returns 0
 */
int keyboard_handler(unsigned long type);

/**
 * @brief Handles mouse interrupts
 *
 * @return Returns 0
 */
int mouse_handler();

/**
 * @brief Handles timer interrupts
 *
 * @return Returns 0
 */
int timer_handler();

/**
 * @brief Handles score information
 *
 * @return Returns 0
 */
int score_handler();

/**
 * @brief Handles time information
 *
 * @return Returns 0
 */
int time_handler();

/**
 * @brief Handles bullets information
 *
 * @return Returns 0
 */
int bullets_handler();

/**
 * @brief Handles lives information
 *
 * @return Returns 0
 */
int lives_handler();

#endif /* _GAME_H */
