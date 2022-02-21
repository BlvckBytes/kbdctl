#ifndef keyboard_key_color_h
#define keyboard_key_color_h

#include "keyboard_key.h"
#include "keyboard_color.h"
#include "util/mman.h"

/**
 * @brief Groups a key to it's requested color
 */
typedef struct keyboard_key_color
{
  keyboard_key_t key;         // Key to change
  keyboard_color_t color;     // Target color
} keyboard_key_color_t;

/**
 * @brief Allocate a new keyboard key color struct
 * 
 * @param key Key to change
 * @param color Target color
 * @return keyboard_key_color_t* Structure instance, mman managed
 */
keyboard_key_color_t *keyboard_key_color_make(keyboard_key_t key, keyboard_color_t color);

#endif