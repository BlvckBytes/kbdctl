#ifndef keyboard_boot_mode_h
#define keyboard_boot_mode_h

/**
 * @brief What mode to display when booting?
 */
typedef enum keyboard_boot_mode
{
  BOOT_STORAGE          = 0x01,         // Boot into the user-defined storage
  BOOT_FACTORY          = 0x02          // Boot into the mode it shipped with
} keyboard_boot_mode_t;

#endif