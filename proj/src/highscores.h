#ifndef _HIGHSCORES_H_
#define _HIGHSCORES_H_

/** @defgroup highscores highscores
 * @{
 *
 * Functions that handle the highscores and highscores file
 */

/**
 * @brief Writes the highscores to the highscores.txt file
 *
 * @return Returns 0
 */
int writeHighscoresFile();

/**
 * @brief Reads the highscores from the highscores.txt file
 *
 * @return Returns 0
 */
int readHighscoresFile();

/**
 * @brief Handles the highscores
 *
 * This function changes the 3 top highscores according to what was the player last score
 *
 * @return Returns 0
 */
int highscore_handler(int score_points);

/**
 * @brief Displays the highscores in the screen
 *
 * @return Returns 0
 */
int draw_highscores();


#endif /* _HIGHSCORES_H */
