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

  scptr uint8_t *data_keys = ctl_frame_make(TYPE_ITEMS);
  keyboard_key_color_t keys[] = {
    { KEY_ALT_LEFT, { 0xFF, 0x00, 0x00 } }
  };

  size_t keys_offs = 0;
  ctl_frame_key_list_apply(data_keys, keys, sizeof(keys) / sizeof(keyboard_key_color_t), &keys_offs);

  if (!keyboard_transmit(kb, data_keys, mman_fetch_meta(data_keys)->num_blocks))
    fprintf(stderr, "Could not transmit data!\n");

  scptr uint8_t *data_statuses = ctl_frame_make(TYPE_ITEMS);
  keyboard_status_color_t statuses[] = {
    { STATUS_BACKLIGHT, { 0x00, 0x00, 0xFF } }
  };

  size_t statuses_offs = 0;
  ctl_frame_status_list_apply(data_statuses, statuses, sizeof(statuses) / sizeof(keyboard_status_color_t), &statuses_offs);

  if (!keyboard_transmit(kb, data_statuses, mman_fetch_meta(data_statuses)->num_blocks))
    fprintf(stderr, "Could not transmit data!\n");

  scptr uint8_t *data_comm = ctl_frame_make(TYPE_COMMIT);
  if (!keyboard_transmit(kb, data_comm, mman_fetch_meta(data_comm)->num_blocks))
    fprintf(stderr, "Could not transmit data!\n");

  keyboard_close(kb);
  hid_exit();
  return 0;
}
