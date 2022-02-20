#ifndef ctl_frame_h
#define ctl_frame_h

#include <inttypes.h>
#include "mman.h"

/* 
  20 byte frame structure:
  0         -> ctl_frame_size
  1         -> 0xFF (delimiter?)
  2, 3      -> ctl_frame_type
  4         -> ctl_frame_target
  5         -> ctl_frame_effect
  6, 7, 8   -> ctl_frame_color
  9, 10     -> ctl_frame_time_t of BREATHING
  11, 12    -> ctl_frame_time_t of CYCLE
  13        -> ctl_frame_wave_type_t
  14        -> 0x64 (unused?)
  15        -> uint8_t as MSB of ctl_frame_time_t (LSB is 12 (shared?)) of WAVE
  16        -> 0x00 (unused?)
  17        -> 0x00 (unused?)
  18        -> 0x00 (unused?)
  19        -> 0x00 (unused?)
*/

/*
  64 byte frame structure:
  // TODO: Reverse engineer
*/

/**
 * @brief The first byte of the frame dictates it's total number of bytes
 */
typedef enum ctl_frame_size
{
  TWENTY_BYTES        = 0x11,         // 20 bytes in total
  SIXTYFOUR_BYTES     = 0x12          // 64 bytes in total
} ctl_frame_size_t;

/**
 * @brief The third and fourth bytes of the frame dictate what
 * command will be executed and thus what parameters it contains
 */
typedef enum ctl_frame_type
{
  TYPE_KEYS           = 0x0C3A,       // Control per-key or key-groups
  TYPE_COMMIT         = 0x0C5A,       // Commit key-changes
  TYPE_EFFECTS        = 0x0D3C,       // Control built-in effects
  TYPE_DEACTIVATE     = 0x0D3E,       // Deactivate the lighting
  TYPE_BOOT_MODE      = 0x0D5A        // Set up into which mode the kbd will boot
} ctl_frame_type_t;

/**
 * @brief Where will the change be applied to?
 */
typedef enum ctl_frame_target
{
  TARG_KEYS           = 0x00,         // This frame will change key lighting
  TARG_LOGO           = 0x01          // This frame will change logo lighting
} ctl_frame_target_t;

/**
 * @brief What direction should the wave animate in?
 */
enum ctl_frame_wave_type
{
  _WAVE_CYCLE            = 0x00,         // Cycle through all wave modes
  _WAVE_HORIZONTAL       = 0x01,         // Horizontal wave from left to right
  _WAVE_HORIZONTAL_REV   = 0x06,         // Horizontal wave from right to left
  _WAVE_VERTICAL         = 0x02,         // Vertical wave from top to bottom
  _WAVE_VERTICAL_REV     = 0x07,         // Vertical wave from bottom to top
  _WAVE_CIRC_CENTER_OUT  = 0x03,         // Circular wave from the inside out
  _WAVE_CIRC_CENTER_IN   = 0x08          // Circular wave from the outside in
};

/**
 * @brief What effect should be applied?
 * INFO: Bitmasking is only done for convenience and not done in the
 * INFO: protocol, hence separate enumerations
 */
typedef enum ctl_frame_effect
{
  EFFECT_COLOR                 = 0x01,                                           // Static color
  EFFECT_BREATHING             = 0x02,                                           // Breathing static color
  EFFECT_CYCLE                 = 0x03,                                           // Cycle through all colors
  _EFFECT_WAVE                 = 0x04,                                           // Wave mode, only used for bitmasking
  EFFECT_WAVE_CYCLE            = _EFFECT_WAVE | (_WAVE_CYCLE            << 8),   // Wave effect (see corresponding type)
  EFFECT_WAVE_HORIZONTAL       = _EFFECT_WAVE | (_WAVE_HORIZONTAL       << 8),   // Wave effect (see corresponding type)
  EFFECT_WAVE_HORIZONTAL_REV   = _EFFECT_WAVE | (_WAVE_HORIZONTAL_REV   << 8),   // Wave effect (see corresponding type)
  EFFECT_WAVE_VERTICAL         = _EFFECT_WAVE | (_WAVE_VERTICAL         << 8),   // Wave effect (see corresponding type)
  EFFECT_WAVE_VERTICAL_REV     = _EFFECT_WAVE | (_WAVE_VERTICAL_REV     << 8),   // Wave effect (see corresponding type)
  EFFECT_WAVE_CIRC_CENTER_OUT  = _EFFECT_WAVE | (_WAVE_CIRC_CENTER_OUT  << 8),   // Wave effect (see corresponding type)
  EFFECT_WAVE_CIRC_CENTER_IN   = _EFFECT_WAVE | (_WAVE_CIRC_CENTER_IN   << 8)    // Wave effect (see corresponding type)
} ctl_frame_effect_t;

/**
 * @brief 24 bit LED color
 */
typedef struct ctl_frame_color
{
  uint8_t r;                           // 0x00-0xFF red value
  uint8_t g;                           // 0x00-0xFF green value
  uint8_t b;                           // 0x00-0xFF blue value
} ctl_frame_color_t;

/**
 * @brief What mode to display when booting?
 */
typedef enum ctl_frame_boot_mode
{
  BOOT_STORAGE          = 0x01,         // Boot into the user-defined storage
  BOOT_FACTORY          = 0x02          // Boot into the mode it shipped with
} ctl_frame_boot_mode_t;

/**
 * @brief Create a zero initialized frame matching a certain type
 * 
 * @param type Type of the frame
 * @return uint8_t* Zero initialized byte array, mman managed
 */
uint8_t *ctl_frame_make(ctl_frame_type_t type);

/**
 * @brief Apply the target byte(s) to a frame
 * 
 * @param frame Frame to apply to
 * @param target Target
 */
void ctl_frame_target_apply(uint8_t *frame, ctl_frame_target_t target);

/**
 * @brief Apply a boot mode to a frame
 * 
 * @param frame Frame to apply to
 * @param boot_mode Boot mode
 */
void ctl_frame_boot_mode_apply(uint8_t *frame, ctl_frame_boot_mode_t boot_mode);

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
void ctl_frame_effect_apply(
  uint8_t *frame,
  ctl_frame_effect_t effect,
  uint16_t time,
  ctl_frame_color_t color,
  bool store
);

#endif