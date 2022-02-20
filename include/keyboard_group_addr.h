#ifndef keyboard_group_addr_h
#define keyboard_group_addr_h

/**
 * @brief The group's address a keyboard item is residing in
 */
enum keyboard_group_addr
{
  KGA_STATUS                    = 0x4005,       // Status LEDs (backlight, game-mode)
  KGA_KEY                       = 0x010E        // Individual keys
} keyboard_group_addr_t;

#endif