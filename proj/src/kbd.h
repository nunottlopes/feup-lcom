#ifndef __KBD_H
#define __KBD_H

/** @defgroup kbd kbd
 * @{
 *
 * Functions for using the keyboard
 */

/**
 * @brief Subscribe the keyboard interrupts
 *
 * @return Return 0 upon success and -1 otherwise
 */
int kbd_subscribe_int();

/**
 * @brief Unsubscribe the keyboard interrupts
 *
 * @return Return 0 upon success and -1 otherwise
 */
int kbd_unsubscribe_int();

/**
 * @brief Reads the scancode from the OUT_BUF
 *
 * @return Returns the scancode
 */
unsigned long kbd_test_scan_C();

#endif
