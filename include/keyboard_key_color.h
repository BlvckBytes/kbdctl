#ifndef keyboard_key_color_h
#define keyboard_key_color_h

#include "keyboard_key.h"

/**
 * @brief Groups a key to it's requested color
 */
typedef struct keyboard_key_color
{
  keyboard_key_t key;         // Key to change
  keyboard_color_t color;     // Target color
} keyboard_key_color_t;

#endif