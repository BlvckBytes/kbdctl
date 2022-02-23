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

// Location of the keymap config file (invalid paths are ignored)
// TODO: Don't use hardcoded paths
#define KEYMAP_FLOC "/Users/blvckbytes/.config/kbdctl/keymap.ini"

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
