
#ifndef keyboard_status_color_h
#define keyboard_status_color_h

#include "keyboard_status.h"

/**
 * @brief Groups a status to it's requested color
 */
typedef struct keyboard_status_color
{
  keyboard_status_t status;     // Status to change
  keyboard_color_t color;       // Target color
} keyboard_status_color_t;

#endif