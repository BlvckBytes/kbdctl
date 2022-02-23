#ifndef keyboard_prompt_h
#define keyboard_prompt_h

#include "util/compattrs.h"
#include "util/partial_strdup.h"
#include "util/enumlut.h"
#include "util/longp.h"
#include "keyboard_devman.h"
#include "keyboard.h"
#include "keyboard_ctl_frame_target.h"
#include "keyboard_effect.h"
#include "keyboard_color.h"
#include "keyboard_ctl_frame.h"

typedef struct keyboard_prompt_state
{
  keyboard_t *kb;
  bool prompting;
} keyboard_prompt_state_t;

char *keyboard_prompt_process(char *input, keyboard_prompt_state_t *state);

keyboard_prompt_state_t *keyboard_prompt_state_make();

#endif