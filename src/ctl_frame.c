#include "ctl_frame.h"

uint8_t *ctl_frame_make(ctl_frame_type_t type)
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
    case TYPE_ITEMS:
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

void ctl_frame_target_apply(uint8_t *frame, ctl_frame_target_t target)
{
  frame[4] = target;
}

void ctl_frame_boot_mode_apply(uint8_t *frame, keyboard_boot_mode_t boot_mode)
{
  frame[5] = 0x01;
  frame[6] = boot_mode;
}

void ctl_frame_effect_apply(
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

/**
 * @brief Inserts an item into a frame based on the current frame-index
 * and the item's type, modifies the frame_index in place
 * 
 * @param frame Frame to insert into, uint8_t *
 * @param frame_index Frame index tracker, size_t *
 * @param items List of items to append, (keyboard_key_color_t | keyboard_status_color_t) *
 * @param type Type of items, key or status
 * @param i Index in list of items, size_t
 */
#define CTL_FRAME_INSERT_ITEM(frame, frame_ind, items, type, i) \
  { \
    frame[frame_ind++] = items[i]->type; \
    frame[frame_ind++] = items[i]->color.r; \
    frame[frame_ind++] = items[i]->color.g; \
    frame[frame_ind++] = items[i]->color.b; \
  }

void ctl_frame_key_list_apply(
  uint8_t *frame,
  keyboard_key_color_t **keys,
  size_t num_keys,
  size_t *keys_offs
)
{
  // Apply group address
  frame[5] = (KGA_KEY >> 8) & 0xFF;
  frame[7] = (KGA_KEY >> 0) & 0xFF;

  size_t frame_ind = 8;
  for(; *keys_offs < num_keys && frame_ind < 63; (*keys_offs)++)
    CTL_FRAME_INSERT_ITEM(frame, frame_ind, keys, key, *keys_offs);
}

void ctl_frame_status_list_apply(
  uint8_t *frame,
  keyboard_status_color_t **statuses,
  size_t num_statuses,
  size_t *statuses_offs
)
{
  // Apply group address
  frame[5] = (KGA_STATUS >> 8) & 0xFF;
  frame[7] = (KGA_STATUS >> 0) & 0xFF;

  size_t frame_ind = 8;
  for(; *statuses_offs < num_statuses && frame_ind < 63; (*statuses_offs)++)
    CTL_FRAME_INSERT_ITEM(frame, frame_ind, statuses, status, *statuses_offs);
}