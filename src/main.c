#include <stdio.h>
#include <hidapi/hidapi.h>

#include "mman.h"
#include "keyboard.h"
#include "strconv.h"

#define TKB_VID 0x046D
#define TKB_PID 0xC339

int main(void)
{
  struct hid_device_info *henum, *dev;
  hid_init();
  henum = hid_enumerate(0x0, 0x0);

  // Loop all known local devices
  scptr keyboard_t *kb = NULL;
  for (dev = henum; dev; dev = dev->next)
  {
    // Skip devices that are not of interest
    if (dev->vendor_id != TKB_VID || dev->product_id != TKB_PID)
      continue;

    // Convert and store
    kb = keyboard_from_hdi(dev);

    // Take the first device for now
    break;
  }

  hid_free_enumeration(henum);

  // No keyboard found
  if (!kb)
  {
    fprintf(stderr, "Could not find the keyboard matching vid=%d and pid=%d!\n", TKB_VID, TKB_PID);
    hid_exit();
    return 1;
  }

  // Print keyboard
  printf("Using the following keyboard:\n");
  keyboard_print(kb);

  if (!keyboard_open(kb))
  {
    hid_exit();
    return 1;
  }

  /*
  // Protocol format (to my current knowledge)
  0:    0x11      Unknown
  1:    0xFF      Unknown
  2:    0x0D      Unknown
  3:    0x3C      Unknown
  4:    DEST      0x01 -> logo, 0x00 -> keys
  5:    EFFECT    0x01 -> color, 0x02 -> breathing, 0x03 -> cycle, 0x04 -> wave
  6:    R         0x00-0xFF
  7:    G         0x00-0xFF
  8:    B         0x00-0xFF
  9:    TIME A    0x00-0xFF (MSB)
  10:   TIME A    0x00-0xFF (LSB)
  11:   TIME B    0x00-0xFF (MSB)
  12:   TIME B    0x00-0xFF (LSB)
  13:   WAV_TYP   0x00 -> wave cycle, 0x01 -> hwave, 0x02 -> vwave, 0x03 -> cwave
  14:   0x64      Unknown
  15:   TIME C    floor(TIME / 256) 0x00-0xFF - no idea what this means yet
  16:   0x00      Unknown
  17:   0x00      Unknown
  18:   0x00      Unknown
  19:   0x00      Unknown
  */

  uint32_t color = 0x0000FF;
  uint16_t time_a = 0x00FF, time_b = 0x0000;
  
  uint8_t data[] = {
    0x11, 0xFF, 0x0D, 0x3C,
    0x01, // Keys
    0x02, // Breathing
    (color >> 16) & 0xFF,   // |
    (color >> 8)  & 0xFF,   // |
    (color >> 0)  & 0xFF,   // +- Color
    (time_a >> 8) & 0xFF,   // |
    (time_a >> 0) & 0xFF,   // +- Time A
    (time_b >> 8) & 0xFF,   // |
    (time_b >> 0) & 0xFF,   // +- Time B
    0x00, // wave cycle
    0x64,
    0x00, // Time C
    0x00, 0x00, 0x00, 0x00
  };

  if (!keyboard_transmit(kb, data, sizeof(data)))
    fprintf(stderr, "Could not transmit data!\n");
  else printf("Applied data!\n");

  keyboard_close(kb);
  hid_exit();
  return 0;
}
