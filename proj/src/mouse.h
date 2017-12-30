#ifndef __MOUSE_H
#define __MOUSE_H

/** @defgroup mouse mouse
 * @{
 *
 * Functions for using the mouse
 */

/** @name  Mouse Struct*/
/**@{
 *
 * Struct that has the mouse x and y position and the information of the left button
 */
typedef struct
{
	int x; //x_position
	int y; //y_position
	int LB;	//left button
}mouse;
/** @} end of Mouse Struct */

/**
 * @brief Subscribe the mouse interrupts
 *
 * @return Return 0 upon success and -1 otherwise
 */
int mouse_subscribe_int();

/**
 * @brief Unsubscribe the mouse interrupts
 *
 * @return Return 0 upon success and -1 otherwise
 */
int mouse_unsubscribe_int();

/**
 * @brief Handles mouse packets
 *
 * @param mouse_current_x is current x position of the mouse
 * @param mouse_current_y is current y position of the mouse
 *
 * @return Returns a mouse status value that is handled in mouse_handler() function
 */
int mouse_game_handler(int *mouse_current_x, int *mouse_current_y);

/**
 * @brief Starts the mouse
 *
 * Sets mouse stream mode and enabled data reporting
 *
 * @return Return 0
 */
int start_mouse();

/**
 * @brief Ends the mouse
 *
 * Disables data reporting and sets mouse stream mode
 *
 * @return Return 0
 */
int end_mouse();

/**
 * @brief Writes to the mouse
 *
 * * @param codigo is what we want to send to the mouse
 *
 * @return Return 0
 */
int send_command(long codigo);


#endif /* __MOUSE_H */
