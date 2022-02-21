
#ifndef keyboard_status_color_h
#define keyboard_status_color_h

#include "keyboard_status.h"
#include "keyboard_color.h"
#include "util/mman.h"

/**
 * @brief Groups a status to it's requested color
 */
typedef struct keyboard_status_color
{
  keyboard_status_t status;     // Status to change
  keyboard_color_t color;       // Target color
} keyboard_status_color_t;

/**
 * @brief Allocate a new keyboard status color struct
 * 
 * @param status Status to chang
 * @param color Target color
 * @return keyboard_key_color_t* Structure instance, mman managed
 */
keyboard_status_color_t *keyboard_status_color_make(keyboard_status_t status, keyboard_color_t color);

#endif