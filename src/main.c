#include <stdio.h>
#include <hidapi/hidapi.h>
#include <unistd.h>

#include "util/mman.h"
#include "util/strconv.h"
#include "util/dynarr.h"
#include "util/htable.h"
#include "util/iniparse.h"
#include "util/dbglog.h"
#include "keyboard.h"
#include "keyboard_ctl_frame.h"
#include "keyboard_devman.h"
#include "keyboard_keymapper.h"
#include "keyboard_animation.h"
#include "keyboard_prompt.h"

// Vendor- and product-id of the target device
#define TKB_VID 0x046D
#define TKB_PID 0xC339

// Location of the keymap config file (invalid paths are ignored)
#define KEYMAP_FLOC "/Users/blvckbytes/.config/kbdctl/keymap.ini"
#define ANIM_FLOC "/Users/blvckbytes/.config/kbdctl/animation.ini"

/*
============================================================================
                               Test functions                               
============================================================================
*/

INLINED static void test_apply_effect
(
  keyboard_t *kb,
  keyboard_effect_t effect,
  keyboard_ctl_frame_target_t target,
  uint16_t time,
  keyboard_color_t color
)
{
  // Create parameterized effect frame and send
  scptr uint8_t *data = keyboard_ctl_frame_make(TYPE_EFFECT);
  keyboard_ctl_frame_effect_apply(data, effect, time, color, true);
  keyboard_ctl_frame_target_apply(data, target);
  if (!keyboard_transmit(kb, data, mman_fetch_meta(data)->num_blocks))
    dbgerr("Could not transmit data!\n");
}

INLINED static void test_apply_status_color
(
  keyboard_t *kb,
  keyboard_color_t color
)
{
  // Create items frame and both available status colors
  scptr uint8_t *data = keyboard_ctl_frame_make(TYPE_KEYS);
  scptr keyboard_key_color_t *stat_backl = keyboard_key_color_make(KEY_STATUS_BACKLIGHT, color);
  scptr keyboard_key_color_t *stat_game = keyboard_key_color_make(KEY_STATUS_GAME, color);
  keyboard_key_color_t *keys_arr[] = {stat_backl, stat_game};

  size_t num_keys = sizeof(keys_arr) / sizeof(keyboard_key_color_t *), statuses_offs = 0;
  keyboard_ctl_frame_key_list_apply(data, keys_arr, num_keys, KGA_STATUS, &statuses_offs);
  if (!keyboard_transmit(kb, data, mman_fetch_meta(data)->num_blocks))
    dbgerr("Could not transmit data!\n");

  // Commit changes and thus make them visible
  scptr uint8_t *data_comm = keyboard_ctl_frame_make(TYPE_COMMIT);
  if (!keyboard_transmit(kb, data_comm, mman_fetch_meta(data_comm)->num_blocks))
    dbgerr("Could not transmit data!\n");
}

INLINED static void test_loop_keys(
  keyboard_t *kb,
  keyboard_color_t color,
  uint16_t delay,
  bool red_logo_before_setting
)
{
  // Create dynamic array where all key colors are stored
  scptr dynarr_t *keys = dynarr_make(16, 256, mman_dealloc_nr);

  // Iterate full range of keys
  for (size_t i = KEY_A; i <= KEY_WIN_RIGHT; i++)
  {
    // Create current key-color and push into array
    scptr keyboard_key_color_t *key_color = keyboard_key_color_make(i, color);

    // Could not push the color
    if (dynarr_push(keys, mman_ref(key_color), NULL) != dynarr_SUCCESS)
      continue;

    // Get current key array state
    scptr keyboard_key_color_t **key_arr = NULL;
    size_t num_keys = dynarr_as_array(keys, (void ***) &key_arr);

    // Make items frame
    scptr uint8_t *data_keys = keyboard_ctl_frame_make(TYPE_KEYS);

    // Make the logo flash up red before setting the key (to find non-available keys)
    if (red_logo_before_setting)
    {
      test_apply_effect(kb, EFFECT_COLOR, TARG_LOGO, 0, (keyboard_color_t) { 0xFF, 0x00, 0x00 });
      usleep(1000 * 500);
      test_apply_effect(kb, EFFECT_COLOR, TARG_LOGO, 0, (keyboard_color_t) { 0x00, 0x00, 0xFF });
      usleep(1000 * 10);
    }

    // Append all keys and send, may take multiple frames as one frame has limited capacity
    size_t keys_offs = 0;
    while (num_keys != keys_offs)
    {
      keyboard_ctl_frame_key_list_apply(data_keys, key_arr, num_keys, KGA_KEY, &keys_offs);
      if (!keyboard_transmit(kb, data_keys, mman_fetch_meta(data_keys)->num_blocks))
        dbgerr("Could not transmit data!\n");
      usleep(1000 * 10);
    }

    // Commit changes and thus make them visible
    scptr uint8_t *data_comm = keyboard_ctl_frame_make(TYPE_COMMIT);
    if (!keyboard_transmit(kb, data_comm, mman_fetch_meta(data_comm)->num_blocks))
      dbgerr("Could not transmit data!\n");

    // Short delay between keys
    usleep(1000 * delay);

    // Skip gap
    if (i == KEY_MENU) i = KEY_CTRL_LEFT - 1;
  }
}

INLINED static void test_boot_mode(keyboard_t *kb, keyboard_boot_mode_t mode)
{
  // Create parameterized boot mode frame and send
  scptr uint8_t *data = keyboard_ctl_frame_make(TYPE_BOOT_MODE);
  keyboard_ctl_frame_boot_mode_apply(data, mode);
  if (!keyboard_transmit(kb, data, mman_fetch_meta(data)->num_blocks))
    dbgerr("Could not transmit data!\n");
}

INLINED static void test_deactivate(keyboard_t *kb, keyboard_ctl_frame_target_t target)
{
  // Create parameterized boot mode frame and send
  scptr uint8_t *data = keyboard_ctl_frame_make(TYPE_DEACTIVATE);
  keyboard_ctl_frame_target_apply(data, target);
  if (!keyboard_transmit(kb, data, mman_fetch_meta(data)->num_blocks))
    dbgerr("Could not transmit data!\n");
}

/*
============================================================================
                                Main program                                
============================================================================
*/

int process(void)
{
  scptr keyboard_prompt_state_t *kbs = keyboard_prompt_state_make();
  while (kbs->prompting)
  {
    char *line = NULL;
    size_t line_len = 0;

    // Try to read a line from STDIN
    if (getline(&line, &line_len, stdin) <= 0)
    {
      free(line);
      continue;
    }

    // Process this request
    scptr char *req = strtrim(line);
    scptr char *answ = keyboard_prompt_process(req, kbs);
    free(line);

    // Print response
    printf("%s", answ);
  }

  // As I'm currently working on the prompt, just comment it out for "normal operation"
  return 0;

  // List available devices
  scptr char *list = keyboard_devman_list();
  dbginf("Available devices:\n%s\n", list);

  // Parse and print the keymap
  scptr char *keymap_err = NULL;
  scptr htable_t *keymap = keyboard_keymapper_load(KEYMAP_FLOC, &keymap_err);
  if (!keymap)
    dbgerr("ERROR: Could not parse the keymap at " QUOTSTR ": %s\n", KEYMAP_FLOC, keymap_err);
  else
  {
    scptr char *parsed = iniparse_dump(keymap);
    dbginf("Parsed keymap table:\n%s\n", parsed);
  }

  // Parse and print the animation
  scptr char *anim_err = NULL;
  scptr keyboard_animation_t *anim = keyboard_animation_load(ANIM_FLOC, &anim_err);
  if (!anim)
    dbgerr("ERROR: Could not parse the animation at " QUOTSTR ": %s\n", ANIM_FLOC, anim_err);
  else
  {
    scptr char *parsed = iniparse_dump(anim->ini);
    dbginf("Parsed animation table:\n%s\n", parsed);
    dbginf(
      "Parsed animation frame_delay=%ld, draw_mode=%s, last_frame=%lu, mapping_lang=%s\n",
      anim->frame_del, keyboard_draw_mode_name(anim->draw_mode), anim->last_frame, anim->mapping_lang
    );
  }

  // No keyboard found
  scptr keyboard_t *kb = keyboard_devman_find(TKB_VID, TKB_PID, NULL);
  if (!kb)
  {
    dbgerr("ERROR: Could not find the keyboard matching vid=%d and pid=%d!\n", TKB_VID, TKB_PID);
    hid_exit();
    return 1;
  }

  // Print keyboard
  scptr char *kb_dump = keyboard_dump(kb);
  dbginf("Using the following keyboard:\n%s\n", kb_dump);

  // Open keyboard connection
  scptr char *kbop_err = NULL;
  if (!keyboard_open(kb, &kbop_err))
  {
    dbgerr("ERROR: %s\n", kbop_err);
    hid_exit();
    return 1;
  }

  // if (keyboard_animation_launch(anim, keymap, kb))
  // {
  //   // Just tessting whether quitting works properly, comment this out for infinite animation
  //   // usleep(1000 * 1000 * 10);
  //   keyboard_animation_quit(anim);
  // }

  hid_exit();
  return 0;
}

int main(void)
{
  // Wrap the process into a separate function to have proper
  // mman deallocation before mman info debugging. This helps
  // me tremendously to find memory-leaks
  int ret = process();
  mman_print_info();
  return ret;
}
