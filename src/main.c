#include <stdio.h>
#include <hidapi/hidapi.h>

#include "mman.h"
#include "keyboard.h"

#define TKB_VID 0x046D
#define TKB_PID 0xC339

int main(void)
{
  struct hid_device_info *henum, *dev;
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
    return 1;
  }

  // Print keyboard
  keyboard_print(kb);
  return 0;
}
