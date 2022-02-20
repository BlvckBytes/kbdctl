#include <stdio.h>
#include <hidapi/hidapi.h>
#include <unistd.h>

#include "mman.h"
#include "keyboard.h"
#include "strconv.h"
#include "ctl_frame.h"
#include "dynarr.h"

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

  // TEST: Color in all known keys, one after the other

  // Create dynamic array where all key colors are stored
  scptr dynarr_t *keys = dynarr_make(16, 256, mman_dealloc);

  // Iterate full range of keys
  for (size_t i = KEY_A; i <= KEY_WIN_RIGHT; i++)
  {
    // Create current key-color and push into array
    keyboard_key_color_t *key_color = keyboard_key_color_make(i, (keyboard_color_t) { 0x00, 0xFF, 0x00 });
    dynarr_push(keys, key_color, NULL);

    // Get current key array state
    scptr keyboard_key_color_t **key_arr = NULL;
    size_t num_keys = dynarr_as_array(keys, (void ***) &key_arr);

    // Make items frame
    scptr uint8_t *data_keys = ctl_frame_make(TYPE_ITEMS);

    // Append all keys and send, may take multiple frames as one frame has limited capacity
    size_t keys_offs = 0;
    while (num_keys != keys_offs)
    {
      ctl_frame_key_list_apply(data_keys, key_arr, num_keys, &keys_offs);
      if (!keyboard_transmit(kb, data_keys, mman_fetch_meta(data_keys)->num_blocks))
        fprintf(stderr, "Could not transmit data!\n");
    }

    // Commit changes and thus make them visible
    scptr uint8_t *data_comm = ctl_frame_make(TYPE_COMMIT);
    if (!keyboard_transmit(kb, data_comm, mman_fetch_meta(data_comm)->num_blocks))
      fprintf(stderr, "Could not transmit data!\n");

    // Short delay
    usleep(1000 * 100);

    // Skip gap
    if (i == KEY_MENU) i = KEY_CTRL_LEFT - 1;
  }

  keyboard_close(kb);
  hid_exit();
  return 0;
}
