#include "keyboard_key_color.h"

keyboard_key_color_t *keyboard_key_color_make(keyboard_key_t key, keyboard_color_t color)
{
  keyboard_key_color_t *res = (keyboard_key_color_t *) mman_alloc(sizeof(keyboard_key_color_t), 1, NULL);
  res->key = key;
  res->color = color;
  return mman_ref(res);
}