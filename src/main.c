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

  printf("Got a connection, could execute commands now!\n");

  // This means "horizontal rgb wave"
  // I still need to learn how this protocol works using other projects, take it as a magic sequence for now...
  uint8_t data[] = {
    0x11, 0xff, 0x0d, 0x3c, 0x00, 0x04, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x0a, 0x00, 0x01, 0x64, 0x0a, 0x00, 0x00, 0x00, 0x00
  };

  if (!keyboard_transmit(kb, data, sizeof(data)))
    fprintf(stderr, "Could not transmit data!\n");

  keyboard_close(kb);
  hid_exit();
  return 0;
}
