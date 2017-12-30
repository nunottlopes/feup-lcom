#ifndef __READ_XPM_H
#define __READ_XPM_H

#define HRES	1024
#define VRES	768

/** @defgroup read_xpm read_xpm
 * @{
 *
 * Function to read xpm file
 */

/**
 *   @Brief Code provided in class used, to parse a xpm type image into a
 *	 char array to use in sprites
 */

/** Reads a xpm-like sprite defined in "map" (look at pixmap.h for
 * examples). Returns the address of the allocated memory where the
 * sprite was read. Updates "width" and "heigh" with the sprite
 * dimension.  Return NULL on error.
 * Assumes that VRES and HRES (the screen vertical and horizontal resolution)
 * are externaly defined.
 *
 * Usage example, using the defined sprite in pixmap.h:
 * <pre>
 *   #include "pixmap.h" // defines  pic1, pic2, etc
 *   int wd, hg;
 *   char *sprite = read_xpm(pic1, &wd, &hg);
 * </pre>
*/

char *read_xpm(char *map[], int *wd, int *ht);

#endif
