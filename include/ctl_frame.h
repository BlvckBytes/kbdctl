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
  EFFECTS             = 0x0D3C,       // Control built-in effects
  DEACTIVATE          = 0x0D3E,       // 
  KEYS                = 0x0C3A,       // Control per-key or key-groups
  COMMIT              = 0x0C5A        // Commit key-changes
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
 * @brief What effect should be applied?
 */
typedef enum ctl_frame_effect
{
  COLOR               = 0x01,         // Static color
  BREATHING           = 0x02,         // Breathing static color
  CYCLE               = 0x03,         // Cycle through all colors
  WAVE                = 0x04,         // Multicolor wave
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
 * @brief What direction should the wave animate in?
 */
typedef enum ctl_frame_wave_type
{
  WAVE_CYCLE            = 0x00,         // Cycle through all wave modes
  WAVE_HORIZONTAL       = 0x01,         // Horizontal wave from left to right
  WAVE_HORIZONTAL_REV   = 0x06,         // Horizontal wave from right to left
  WAVE_VERTICAL         = 0x02,         // Vertical wave from top to bottom
  WAVE_VERTICAL_REV     = 0x07,         // Vertical wave from bottom to top
  WAVE_CIRC_CENTER_OUT  = 0x03,         // Circular wave from the inside out
  WAVE_CIRC_CENTER_IN   = 0x08,         // Circular wave from the outside in

  NOT_A_WAVE            = 0xFF          // Use this value for non-wave frames
} ctl_frame_wave_type_t;

/**
 * @brief Create a zero initialized frame matching a certain type
 * 
 * @param type Type of the frame
 * @return uint8_t* Zero initialized byte array, mman managed
 */
uint8_t *ctl_frame_make(ctl_frame_type_t type);

void ctl_frame_target_apply(uint8_t *frame, ctl_frame_target_t target);

void ctl_frame_effect_dynamic_apply(
  uint8_t *frame,
  ctl_frame_effect_t effect,
  uint16_t time,
  ctl_frame_color_t color,
  ctl_frame_wave_type_t wave_type
);

void ctl_frame_effect_static_apply(
  uint8_t *frame,
  ctl_frame_color_t color
);

#endif