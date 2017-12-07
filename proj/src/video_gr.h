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

void * vg_open(unsigned short mode);

int paint_pixel(unsigned short x, unsigned short y, unsigned long color);

int draw_pixmap(char *pixmap, short x_min, short y_min, int width, int height);

int clean_screen();

int draw_time_score_bar();

int draw_number(int number, int x, int y);

int draw_bullets(int number);

int draw_scope(int x, int y);

int clean_scope(int x, int y);

 /** @} end of video_gr */
 
#endif /* __VIDEO_GR_H */
