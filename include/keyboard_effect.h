#ifndef keyboard_effect_h
#define keyboard_effect_h

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

/**
 * @brief What effect should be applied?
 * INFO: Bitmasking is only done for convenience and not done in the
 * INFO: protocol, hence separate enumerations
 */
typedef enum keyboard_effect
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
} keyboard_effect_t;

#endif