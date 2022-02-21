#include <stdio.h>
#include <hidapi/hidapi.h>
#include <unistd.h>

#include "util/mman.h"
#include "keyboard.h"
#include "util/strconv.h"
#include "ctl_frame.h"
#include "util/dynarr.h"
#include "util/htable.h"
#include "util/iniparse.h"

// Vendor- and product-id of the target device
#define TKB_VID 0x046D
#define TKB_PID 0xC339

// Location of the keymap config file (invalid paths are ignored)
#define KEYMAP_FLOC "/Users/blvckbytes/.config/kbdctl/keymap.ini"

// TEST: Color in all known keys, one after the other
INLINED static void test_loop_keys(keyboard_t *kb, keyboard_color_t color)
{
  // Create dynamic array where all key colors are stored
  scptr dynarr_t *keys = dynarr_make(16, 256, mman_dealloc_nr);

  // Iterate full range of keys
  for (size_t i = KEY_A; i <= KEY_WIN_RIGHT; i++)
  {
    // Create current key-color and push into array
    scptr keyboard_key_color_t *key_color = keyboard_key_color_make(i, color);

    // Dealloc this color it it didn't fit into the array anymore
    if (dynarr_push(keys, mman_ref(key_color), NULL) != dynarr_SUCCESS)
      mman_dealloc(key_color);

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

    // Short delay between keys
    usleep(1000 * 50);

    // Skip gap
    if (i == KEY_MENU) i = KEY_CTRL_LEFT - 1;
  }
}

INLINED static void test_apply_effect
(
  keyboard_t *kb,
  keyboard_effect_t effect,
  ctl_frame_target_t target,
  uint16_t time,
  keyboard_color_t color
)
{
  // Create parameterized effect frame and send
  scptr uint8_t *data = ctl_frame_make(TYPE_EFFECT);
  ctl_frame_effect_apply(data, effect, time, color, true);
  ctl_frame_target_apply(data, target);
  if (!keyboard_transmit(kb, data, mman_fetch_meta(data)->num_blocks))
    fprintf(stderr, "Could not transmit data!\n");
}

INLINED static void test_apply_status_color
(
  keyboard_t *kb,
  keyboard_color_t color
)
{
  // Create items frame and both available status colors
  scptr uint8_t *data = ctl_frame_make(TYPE_ITEMS);
  scptr keyboard_status_color_t *stat_backl = keyboard_status_color_make(STATUS_BACKLIGHT, color);
  scptr keyboard_status_color_t *stat_game = keyboard_status_color_make(STATUS_GAME, color);
  keyboard_status_color_t *statuses_arr[] = {stat_backl, stat_game};

  size_t num_statuses = sizeof(statuses_arr) / sizeof(keyboard_status_color_t *), statuses_offs = 0;
  ctl_frame_status_list_apply(data, statuses_arr, num_statuses, &statuses_offs);
  if (!keyboard_transmit(kb, data, mman_fetch_meta(data)->num_blocks))
    fprintf(stderr, "Could not transmit data!\n");

  // Commit changes and thus make them visible
  scptr uint8_t *data_comm = ctl_frame_make(TYPE_COMMIT);
  if (!keyboard_transmit(kb, data_comm, mman_fetch_meta(data_comm)->num_blocks))
    fprintf(stderr, "Could not transmit data!\n");
}

INLINED static void test_boot_mode(keyboard_t *kb, keyboard_boot_mode_t mode)
{
  // Create parameterized boot mode frame and send
  scptr uint8_t *data = ctl_frame_make(TYPE_BOOT_MODE);
  ctl_frame_boot_mode_apply(data, mode);
  if (!keyboard_transmit(kb, data, mman_fetch_meta(data)->num_blocks))
    fprintf(stderr, "Could not transmit data!\n");
}

INLINED static void test_deactivate(keyboard_t *kb, ctl_frame_target_t target)
{
  // Create parameterized boot mode frame and send
  scptr uint8_t *data = ctl_frame_make(TYPE_DEACTIVATE);
  ctl_frame_target_apply(data, target);
  if (!keyboard_transmit(kb, data, mman_fetch_meta(data)->num_blocks))
    fprintf(stderr, "Could not transmit data!\n");
}

int process(void)
{
  // Parse and print the keymap
  scptr char *err = NULL;
  scptr htable_t *keymap = iniparse(KEYMAP_FLOC, &err);
  if (!keymap)
    fprintf(stderr, "ERROR: Could not parse the keymap at " QUOTSTR ": %s\n", KEYMAP_FLOC, err);
  else
    iniparse_print(keymap);

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
    fprintf(stderr, "ERROR: Could not find the keyboard matching vid=%d and pid=%d!\n", TKB_VID, TKB_PID);
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

  /* Testcases - nice and manual! :) */

  // test_apply_effect(kb, EFFECT_WAVE_CIRC_CENTER_IN, TARG_KEYS, 800, (keyboard_color_t) { 0x00, 0x00, 0x00 });
  // test_apply_effect(kb, EFFECT_WAVE_CIRC_CENTER_OUT, TARG_KEYS, 800, (keyboard_color_t) { 0x00, 0x00, 0x00 });
  test_apply_effect(kb, EFFECT_WAVE_HORIZONTAL, TARG_KEYS, 800, (keyboard_color_t) { 0x00, 0x00, 0x00 });
  // test_apply_effect(kb, EFFECT_WAVE_HORIZONTAL_REV, TARG_KEYS, 800, (keyboard_color_t) { 0x00, 0x00, 0x00 });
  // test_apply_effect(kb, EFFECT_WAVE_VERTICAL, TARG_KEYS, 800, (keyboard_color_t) { 0x00, 0x00, 0x00 });
  // test_apply_effect(kb, EFFECT_WAVE_VERTICAL_REV, TARG_KEYS, 800, (keyboard_color_t) { 0x00, 0x00, 0x00 });
  // test_apply_effect(kb, EFFECT_BREATHING, TARG_KEYS, 5000, (keyboard_color_t) { 0xFF, 0x00, 0x00 });
  // test_apply_effect(kb, EFFECT_CYCLE, TARG_KEYS, 8000, (keyboard_color_t) { 0x00, 0x00, 0x00 });
  // test_apply_effect(kb, EFFECT_BREATHING, TARG_KEYS, 800, (keyboard_color_t) { 0x00, 0x00, 0xFF });
  // test_apply_effect(kb, EFFECT_BREATHING, TARG_LOGO, 1000, (keyboard_color_t) { 0x00, 0x00, 0xFF });
  // test_apply_status_color(kb, (keyboard_color_t) { 0xFF, 0x00, 0x00 });
  // test_boot_mode(kb, BOOT_FACTORY);
  // test_loop_keys(kb, (keyboard_color_t) { 0x00, 0xFF, 0x00 });
  // test_deactivate(kb, TARG_LOGO);
  // test_deactivate(kb, TARG_KEYS);

  keyboard_close(kb);
  hid_exit();
  return 0;
}

int main(void)
{
  int ret = process();
  mman_print_info();
  return ret;
}
