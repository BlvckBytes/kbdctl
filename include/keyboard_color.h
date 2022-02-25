#ifndef keyboard_color_h
#define keyboard_color_h

#include <inttypes.h>
#include "util/mman.h"
#include "util/strfmt.h"

/**
 * @brief 24 bit LED color
 */
typedef struct keyboard_color
{
  uint8_t r;                           // 0x00-0xFF red value
  uint8_t g;                           // 0x00-0xFF green value
  uint8_t b;                           // 0x00-0xFF blue value
} keyboard_color_t;

/**
 * @brief Apply a number's bytes to the individual R, G and B values of a color
 * 
 * @param color Color to apply to
 * @param number Number to apply
 */
void keyboard_color_apply_number(keyboard_color_t *color, long number);

/**
 * @brief Convert a keyboard color to it's hex-repr, without the leading #
 * 
 * @param color Color to convert
 * @return char* Hex repr
 */
char *keyboard_color_to_hex(keyboard_color_t *color);

#endif