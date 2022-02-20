#ifndef keyboard_color_h
#define keyboard_color_h

#include <inttypes.h>

/**
 * @brief 24 bit LED color
 */
typedef struct keyboard_color
{
  uint8_t r;                           // 0x00-0xFF red value
  uint8_t g;                           // 0x00-0xFF green value
  uint8_t b;                           // 0x00-0xFF blue value
} keyboard_color_t;

#endif