#include "keyboard_ctl_frame.h"

uint8_t *keyboard_ctl_frame_make(keyboard_ctl_frame_type_t type)
{
  scptr uint8_t *res;
  size_t frame_size;

  switch (type)
  {
    // Effects and commits have 20B frames
    case TYPE_EFFECT:
    case TYPE_DEACTIVATE:
    case TYPE_COMMIT:
    case TYPE_BOOT_MODE:
      frame_size = 20;
      res = mman_calloc(sizeof(uint8_t), frame_size, NULL);
      res[0] = TWENTY_BYTES;
      res[14] = 0x64;
      break;

    // Items have 64B frames
    case TYPE_KEYS:
      frame_size = 64;
      res = mman_calloc(sizeof(uint8_t), frame_size, NULL);
      res[0] = SIXTYFOUR_BYTES;
      break;

    // Unknown type
    default:
      return NULL;
  }

  // Delimiter?
  res[1] = 0xFF;

  // Type MSB and LSB
  res[2] = (type >> 8) & 0xFF;
  res[3] = (type >> 0) & 0xFF;

  return mman_ref(res);
}

void keyboard_ctl_frame_target_apply(uint8_t *frame, keyboard_ctl_frame_target_t target)
{
  frame[4] = target;
}

void keyboard_ctl_frame_boot_mode_apply(uint8_t *frame, keyboard_boot_mode_t boot_mode)
{
  frame[5] = 0x01;
  frame[6] = boot_mode;
}

void keyboard_ctl_frame_effect_apply(
  uint8_t *frame,
  keyboard_effect_t effect,
  uint16_t time,
  keyboard_color_t color,
  bool store
)
{
  // Apply effect and mask out meta-data
  frame[5] = effect & 0xFF;

  // Split up 16 bit time value
  uint8_t time_msb = (time >> 8) & 0xFF, time_lsb = (time >> 0) & 0xFF;

  // Apply color bytes if supported
  if (effect == EFFECT_COLOR || effect == EFFECT_BREATHING)
  {
    frame[6] = color.r;
    frame[7] = color.g;
    frame[8] = color.b;
  }

  // Apply the time for all effects (redundant values don't hurt) if supported
  if (effect != EFFECT_COLOR)
  {
    // Breathing time
    frame[9] = time_msb;
    frame[10] = time_lsb;

    // Cycle time
    frame[11] = time_msb;
    frame[12] = time_lsb;

    // Wave time
    frame[15] = time_msb;
  }

  // Apply persistence if requested
  if (store)
    frame[16] = 0x1;

  // Apply the wave type if it's actually a wave
  if (effect & _EFFECT_WAVE)
    frame[13] = (effect >> 8) & 0xFF;
}

void keyboard_ctl_frame_key_list_apply(
  uint8_t *frame,
  keyboard_key_color_t **keys,
  size_t num_keys,
  keyboard_group_addr_t key_group,
  size_t *keys_offs
)
{
  // Apply group address (two MSBs of the key)
  frame[5] = (key_group >> 8) & 0xFF;
  frame[7] = (key_group >> 0) & 0xFF;

  size_t frame_ind = 8;
  for(; *keys_offs < num_keys && frame_ind < 63; (*keys_offs)++)
  {
    frame[frame_ind++] = (keys[*keys_offs]->key) & 0xFF;
    frame[frame_ind++] = keys[*keys_offs]->color.r;
    frame[frame_ind++] = keys[*keys_offs]->color.g;
    frame[frame_ind++] = keys[*keys_offs]->color.b;
  }
}