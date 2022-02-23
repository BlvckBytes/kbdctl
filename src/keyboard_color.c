#include "keyboard_color.h"

void keyboard_color_apply_number(keyboard_color_t *color, long number)
{
  // R  G  B
  // XX XX XX
  color->r = (number >> 16) & 0xFF;
  color->g = (number >> 8)  & 0xFF;
  color->b = (number >> 0)  & 0xFF;
}