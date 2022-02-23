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
                                Main program                                
============================================================================
*/

int process(void)
{
  scptr keyboard_prompt_state_t *kbs = keyboard_prompt_state_make(KEYMAP_FLOC);
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

  // Parse and print the animation
  // scptr char *anim_err = NULL;
  // scptr keyboard_animation_t *anim = keyboard_animation_load(ANIM_FLOC, &anim_err);
  // if (!anim)
  //   dbgerr("ERROR: Could not parse the animation at " QUOTSTR ": %s\n", ANIM_FLOC, anim_err);
  // else
  // {
  //   scptr char *parsed = iniparse_dump(anim->ini);
  //   dbginf("Parsed animation table:\n%s\n", parsed);
  //   dbginf(
  //     "Parsed animation frame_delay=%ld, draw_mode=%s, last_frame=%lu, mapping_lang=%s\n",
  //     anim->frame_del, keyboard_draw_mode_name(anim->draw_mode), anim->last_frame, anim->mapping_lang
  //   );
  // }

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
