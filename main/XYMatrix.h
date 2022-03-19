#ifndef XYMATRX_H
#define XYMATRX_H

/* Includes *****************************************************************/
/* Defines ******************************************************************/
/* Public Functions**********************************************************/

/**
 * @brief Set the Matix Dimension object
 * 
 * @param x   Number of LEDs across
 * @param y   Number of LEDs tall
 */
void SetMatixDimension(uint8_t x, uint8_t y);

/**
 * @brief Return LED value based on X Y matrix. Helper function
 * 
 */
uint16_t XY( uint8_t x, uint8_t y);
#endif