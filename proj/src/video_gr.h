#ifndef __VIDEO_GR_H
#define __VIDEO_GR_H

/** @defgroup video_gr video_gr
 * @{
 *
 * Functions for outputing data to screen in graphics mode
 */

/**
 * @brief Initializes the video module in graphics mode
 * 
 * Uses the VBE INT 0x10 interface to set the desired
 *  graphics mode, maps VRAM to the process' address space and
 *  initializes static global variables with the resolution of the screen, 
 *  and the number of colors
 * 
 * @param mode 16-bit VBE mode to set
 * @return Virtual address VRAM was mapped to. NULL, upon failure.
 */
void *vg_init(unsigned short mode);

 /**
 * @brief Returns to default Minix 3 text mode (0x03: 25 x 80, 16 colors)
 * 
 * @return 0 upon success, non-zero upon failure
 */
int vg_exit(void);

/**
 * @brief Starts Minix video mode
 *
 * @param mode chosen for video
 */
void * vg_open(unsigned short mode);

/**
 * @brief Sets the pixel in the x and y position with a certain color
 *
 * @param x position of the pixel
 * @param y position of the pixel
 * @param color of the pixel
 *
 * @return Return 0 upon success and 1 otherwise
 */
int paint_pixel(unsigned short x, unsigned short y, unsigned long color);

/**
 * @brief Draws a pixmap
 *
 * @param pixmap to be drawn
 * @param x position of the pixmap
 * @param y position of the pixmap
 * @param width of the pixmap
 * @param height of the pixmap
 *
 * @return Return 0
 */
int draw_pixmap(char *pixmap, short x, short y, int width, int height);

/**
 * @brief Puts all screen in black color
 *
 * @return Return 0
 */
int clean_screen();

/**
 * @brief Draws a green bar on the top
 *
 * @return Return 0
 */
int draw_time_score_bar();

/**
 * @brief Draws a number
 *
 * @param number to be drawn
 * @param x position of the number
 * @param y position of the number
 *
 * @return Return 0
 */
int draw_number(int number, int x, int y);

/**
 * @brief Draws the bullets in the top bar
 *
 * @param number of bullets
 *
 * @return Return 0
 */
int draw_bullets(int number);

/**
 * @brief Draws the lives in the top bar
 *
 * @param number of lives
 *
 * @return Return 0
 */
int draw_lives(int number);

/**
 * @brief Draws a scope
 *
 * @param x position of the scope
 * @param y position of the scope
 *
 * @return Return 0
 */
int draw_scope(int x, int y);

/**
 * @brief Erases the scope
 *
 * @param x position of the scope
 * @param y position of the scope
 *
 * @return Return 0
 */
int clean_scope(int x, int y);

 /** @} end of video_gr */
 
#endif /* __VIDEO_GR_H */
