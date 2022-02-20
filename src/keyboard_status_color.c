#include "keyboard_status_color.h"

keyboard_status_color_t *keyboard_status_color_make(keyboard_status_t status, keyboard_color_t color)
{
  keyboard_status_color_t *res = (keyboard_status_color_t *) mman_alloc(sizeof(keyboard_status_color_t), 1, NULL);
  res->status = status;
  res->color = color;
  return mman_ref(res);
}