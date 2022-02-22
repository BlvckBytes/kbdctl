#ifndef keyboard_effect_h
#define keyboard_effect_h

#include "util/enumlut.h"

/**
 * @brief What direction should the wave animate in?
 */
enum _keyboard_wave_type
{
  _WAVE_CYCLE            = 0x00,         // Cycle through all wave modes
  _WAVE_HORIZONTAL       = 0x01,         // Horizontal wave from left to right
  _WAVE_HORIZONTAL_REV   = 0x06,         // Horizontal wave from right to left
  _WAVE_VERTICAL         = 0x02,         // Vertical wave from top to bottom
  _WAVE_VERTICAL_REV     = 0x07,         // Vertical wave from bottom to top
  _WAVE_CIRC_CENTER_OUT  = 0x03,         // Circular wave from the inside out
  _WAVE_CIRC_CENTER_IN   = 0x08          // Circular wave from the outside in
};

// Wave effect base bitmask
#define _EFFECT_WAVE 0x04

// What effect should be applied?
// INFO: Bitmasking is only done for convenience and not done in the
// INFO: protocol, hence separate enumerations
#define _EVALS_KEYBOARD_EFFECT(FUN)                                                \
  FUN(EFFECT_COLOR,                 0x01)                                          \
  FUN(EFFECT_BREATHING,             0x02)                                          \
  FUN(EFFECT_CYCLE,                 0x03)                                          \
  FUN(EFFECT_WAVE_CYCLE,            _EFFECT_WAVE | (_WAVE_CYCLE            << 8))  \
  FUN(EFFECT_WAVE_HORIZONTAL,       _EFFECT_WAVE | (_WAVE_HORIZONTAL       << 8))  \
  FUN(EFFECT_WAVE_HORIZONTAL_REV,   _EFFECT_WAVE | (_WAVE_HORIZONTAL_REV   << 8))  \
  FUN(EFFECT_WAVE_VERTICAL,         _EFFECT_WAVE | (_WAVE_VERTICAL         << 8))  \
  FUN(EFFECT_WAVE_VERTICAL_REV,     _EFFECT_WAVE | (_WAVE_VERTICAL_REV     << 8))  \
  FUN(EFFECT_WAVE_CIRC_CENTER_OUT,  _EFFECT_WAVE | (_WAVE_CIRC_CENTER_OUT  << 8))  \
  FUN(EFFECT_WAVE_CIRC_CENTER_IN,   _EFFECT_WAVE | (_WAVE_CIRC_CENTER_IN   << 8)) 

ENUM_TYPEDEF_FULL_IMPL(keyboard_effect, _EVALS_KEYBOARD_EFFECT);

#endif