#ifndef keyboard_prompt_h
#define keyboard_prompt_h

#include "util/compattrs.h"
#include "util/partial_strdup.h"
#include "util/enumlut.h"
#include "util/longp.h"
#include "util/htable.h"
#include "keyboard_devman.h"
#include "keyboard.h"
#include "keyboard_ctl_frame_target.h"
#include "keyboard_animation.h"
#include "keyboard_keymapper.h"
#include "keyboard_effect.h"
#include "keyboard_color.h"
#include "keyboard_ctl_frame.h"

// TODO: Don't use hardcoded paths
#define KEYMAP_FLOC_BASEPATH "/Users/blvckbytes/.config/kbdctl/animations"

/**
 * @brief Represents the state of the prompter which gets manipulated by commands
 */
typedef struct keyboard_prompt_state
{
  keyboard_t *kb;                           // Targetted keyboard
  htable_t *mappings;                       // Mappings applied on the keys

  htable_t *commands;                       // Command table
  htable_t *usages;                         // Usage table, mapping usages to commands

  keyboard_animation_t *curr_anim;          // Currently playing animation

  bool prompting;                           // Prompting loop state
} keyboard_prompt_state_t;

/**
 * @brief Represents an individual command handler
 */
typedef char *(*keyboard_prompt_command_t)(char *, char *, keyboard_prompt_state_t *);

/**
 * @brief Process user input and modify the state accordingly
 * 
 * @param input User input to process
 * @param state State of the prompt
 * @return char* Response to the user
 */
char *keyboard_prompt_process(char *input, keyboard_prompt_state_t *state);

/**
 * @brief Create a new prompt-state with default values
 * 
 * @param keymap_floc File location of the keymap ini
 */
keyboard_prompt_state_t *keyboard_prompt_state_make(const char *keymap_floc);

#endif