#ifndef keyboard_ctl_frame_h
#define keyboard_ctl_frame_h

#include <inttypes.h>
#include <stddef.h>

#include "util/mman.h"
#include "keyboard_color.h"
#include "keyboard_key_color.h"
#include "keyboard_effect.h"
#include "keyboard_boot_mode.h"
#include "keyboard_group_addr.h"

/* 
  20 byte frame structure:
  0         -> keyboard_ctl_frame_size
  1         -> 0xFF (delimiter?)
  2, 3      -> keyboard_ctl_frame_type
  4         -> keyboard_ctl_frame_target
  5         -> keyboard_ctl_frame_effect
  6, 7, 8   -> keyboard_ctl_frame_color
  9, 10     -> keyboard_ctl_frame_time_t of BREATHING
  11, 12    -> keyboard_ctl_frame_time_t of CYCLE
  13        -> keyboard_ctl_frame_wave_type_t
  14        -> 0x64 (unused?)
  15        -> uint8_t as MSB of keyboard_ctl_frame_time_t (LSB is 12 (shared?)) of WAVE
  16        -> persistence bool (whether or not to store)
  17        -> 0x00 (unused?)
  18        -> 0x00 (unused?)
  19        -> 0x00 (unused?)
*/

/*
  64 byte frame structure:
  0         -> keyboard_ctl_frame_size
  1         -> 0xFF (delimiter?)
  2, 3      -> keyboard_ctl_frame_type
  4         -> 0x00 (unused?)
  5         -> keyboard_group_addr_t MSB
  6         -> 0x00 (unused?)
  7         -> keyboard_group_addr_t LSB
  8 - 63    -> [(keyboard_key_t | keyboard_status_t) r, g, b] * (64 - 8 / 4)
*/

/**
 * @brief The first byte of the frame dictates it's total number of bytes
 */
typedef enum keyboard_ctl_frame_size
{
  TWENTY_BYTES        = 0x11,         // 20 bytes in total
  SIXTYFOUR_BYTES     = 0x12          // 64 bytes in total
} keyboard_ctl_frame_size_t;

/**
 * @brief The third and fourth bytes of the frame dictate what
 * command will be executed and thus what parameters it contains
 */
typedef enum keyboard_ctl_frame_type
{
  TYPE_KEYS           = 0x0C3A,       // Control individual items (keys / statuses)
  TYPE_COMMIT         = 0x0C5A,       // Commit key-changes
  TYPE_EFFECT         = 0x0D3C,       // Control built-in effects
  TYPE_DEACTIVATE     = 0x0D3E,       // Deactivate the lighting
  TYPE_BOOT_MODE      = 0x0D5A        // Set up into which mode the kbd will boot
} keyboard_ctl_frame_type_t;

/**
 * @brief Where will the change be applied to?
 */
typedef enum keyboard_ctl_frame_target
{
  TARG_KEYS           = 0x00,         // This frame will change key lighting
  TARG_LOGO           = 0x01          // This frame will change logo lighting
} keyboard_ctl_frame_target_t;

/**
 * @brief Create a zero initialized frame matching a certain type
 * 
 * @param type Type of the frame
 * @return uint8_t* Zero initialized byte array, mman managed
 */
uint8_t *keyboard_ctl_frame_make(keyboard_ctl_frame_type_t type);

/**
 * @brief Apply the target byte(s) to a frame
 * 
 * @param frame Frame to apply to
 * @param target Target
 */
void keyboard_ctl_frame_target_apply(uint8_t *frame, keyboard_ctl_frame_target_t target);

/**
 * @brief Apply a boot mode to a frame
 * 
 * @param frame Frame to apply to
 * @param boot_mode Boot mode
 */
void keyboard_ctl_frame_boot_mode_apply(uint8_t *frame, keyboard_boot_mode_t boot_mode);

/**
 * @brief Apply all parameter's byte(s) needed for a built-in effect
 * 
 * @param frame Frame to apply to
 * @param effect Effect to display
 * @param time Duration/period of the animation, value is ignored for non-timed effects
 * @param color Color of the effect, is ignored for non-colorable effects
 * @param wave_type Type of the wave, is ignored for non-wave effects
 * @param store Whether or not to write to kbd's persistence
 */
void keyboard_ctl_frame_effect_apply(
  uint8_t *frame,
  keyboard_effect_t effect,
  uint16_t time,
  keyboard_color_t color,
  bool store
);

/**
 * @brief Apply the maximum number of keys and set the last applied offset
 * within the list of keys in order to be picked up by another issued request later
 * 
 * @param frame Frame to apply to
 * @param keys List of keys with their corresponding color
 * @param num_keys Number of keys in that list
 * @param key_group Group these keys are residing in
 * @param keys_offs Offset within the keys list, done when keys_offs == num_keys
 */
void keyboard_ctl_frame_key_list_apply(
  uint8_t *frame,
  keyboard_key_color_t **keys,
  size_t num_keys,
  keyboard_group_addr_t key_group,
  size_t *keys_offs
);

#endif