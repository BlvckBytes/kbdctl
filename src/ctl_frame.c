#include "ctl_frame.h"

uint8_t *ctl_frame_make(ctl_frame_type_t type)
{
  scptr uint8_t *res;
  size_t frame_size;

  switch (type)
  {
    // Effects and commits have 20B frames
    case EFFECTS:
    case DEACTIVATE:
    case COMMIT:
      frame_size = 20;
      res = mman_calloc(sizeof(uint8_t), frame_size, NULL);
      res[0] = TWENTY_BYTES;
      res[14] = 0x64;
      break;

    // Keys have 64B frames
    case KEYS:
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

void ctl_frame_effect_dynamic_apply(
  uint8_t *frame,
  ctl_frame_effect_t effect,
  uint16_t time,
  ctl_frame_color_t color,
  ctl_frame_wave_type_t wave_type
)
{
  frame[5] = effect;

  // Split up 16 bit time value
  uint8_t time_msb = (time >> 8) & 0xFF, time_lsb = (time >> 0) & 0xFF;

  // Apply color bytes
  frame[6] = color.r;
  frame[7] = color.g;
  frame[8] = color.b;

  // Setting the time for all effects (redundant values don't hurt)

  // Breathing time
  frame[9] = time_msb;
  frame[10] = time_lsb;

  // Cycle time
  frame[11] = time_msb;
  frame[12] = time_lsb;

  // Wave time
  frame[15] = time_msb;

  // Apply the wave type if it's actually a wave
  if (effect == WAVE)
    frame[13] = wave_type;
}