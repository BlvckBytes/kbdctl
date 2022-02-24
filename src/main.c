#include <stdio.h>
#include "util/mman.h"
#include "keyboard_prompt.h"

int process(int argc, char **argv)
{
  if (argc < 2)
  {
    fprintf(stderr, "Usage: %s <kbdctl folder absolute path> [socket port]\n", argv[0]);
    return 1;
  }

  // Parse port number if provided, -1 means that the socket is disabled
  long port_l = -1;
  if (argc > 2 && longp(&port_l, argv[2], 10) != LONGP_SUCCESS)
  {
    fprintf(stderr, "Invalid port decimal number: %s\n", argv[2]);
    return 1;
  }

  scptr char *err = NULL;
  scptr keyboard_prompt_state_t *kbs = keyboard_prompt_state_make(argv[1], &err);
  if (!kbs)
  {
    fprintf(stderr, "%s", err);
    return 1;
  }

  while (kbs->prompting)
  {
    scptr char **line = (char **) mman_wrap(NULL, free);
    size_t line_len = 0;

    // Try to read a line from STDIN
    if (getline(line, &line_len, stdin) <= 0)
      continue;

    // Process this request
    scptr char *req = strtrim(*line);
    scptr char *answ = keyboard_prompt_process(req, kbs);

    // Print response
    printf("%s", answ);
  }

  hid_exit();
  return 0;
}

int main(int argc, char **argv)
{
  // Wrap the process into a separate function to have proper
  // mman deallocation before mman info debugging. This helps
  // me tremendously to find memory-leaks
  int ret = process(argc, argv);
  mman_print_info();
  return ret;
}
