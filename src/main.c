#include <stdio.h>
#include <hidapi/hidapi.h>

#include "mman.h"
#include "keyboard.h"
#include "strconv.h"
#include "ctl_frame.h"

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

  scptr uint8_t *data = ctl_frame_make(EFFECTS);
  ctl_frame_effect_apply(
    data,
    WAVE,
    2000,
    (ctl_frame_color_t) { 0, 0, 0 },
    WAVE_CIRC_CENTER_OUT
  );

  ctl_frame_target_apply(data, TARG_KEYS);

  if (!keyboard_transmit(kb, data, mman_fetch_meta(data)->num_blocks))
    fprintf(stderr, "Could not transmit data!\n");

  keyboard_close(kb);
  hid_exit();
  return 0;
}
