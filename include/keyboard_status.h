#ifndef keyboard_status_h
#define keyboard_status_h

#include "keyboard_group_addr.h"

/**
 * @brief Keyboard status LEDs and their corresponding address
 */
typedef enum keyboard_status
{
  STATUS_BACKLIGHT             = 0x01,   // Backlight enable/disable
  STATUS_GAME,                           // Game mode enable/disable
} keyboard_status_t;

#endif