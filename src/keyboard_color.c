#include "keyboard_color.h"

void keyboard_color_apply_number(keyboard_color_t *color, long number)
{
  // R  G  B
  // XX XX XX
  color->r = (number >> 16) & 0xFF;
  color->g = (number >> 8)  & 0xFF;
  color->b = (number >> 0)  & 0xFF;
}

char *keyboard_color_to_hex(keyboard_color_t *color)
{
  scptr char *buf = mman_alloc(sizeof(char), 6, NULL);
  size_t buf_offs = 0;

  strfmt(&buf, &buf_offs, "%02x", color->r);
  strfmt(&buf, &buf_offs, "%02x", color->g);
  strfmt(&buf, &buf_offs, "%02x", color->b);

  return mman_ref(buf);
}