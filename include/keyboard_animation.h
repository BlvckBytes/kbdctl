#ifndef keyboard_animation_h
#define keyboard_animation_h

#include <unistd.h>
#include <pthread.h>

#include "keyboard_draw_mode.h"
#include "keyboard_keymapper.h"
#include "keyboard_ctl_frame.h"
#include "keyboard_key_color.h"
#include "keyboard.h"
#include "util/iniparse.h"
#include "util/htable.h"
#include "util/longp.h"
#include "util/dynarr.h"
#include "util/htable.h"

typedef struct keyboard_animation
{
  htable_t *ini;                        // Full animation INI table

  keyboard_draw_mode_t draw_mode;       // Draw mode to use for data-management between frames
  long last_frame;                      // Last frame specified without gaps
  long curr_frame;                      // Currently displayed frame, 0 = none
  long frame_del;                       // Delay in milliseconds between frames

  pthread_t thread;                     // Executing thread
  bool looping;                         // Whether or not the thread's loop is active
  keyboard_t *device;                   // Device to be executed on
  dynarr_t *framebuffer;                // Framebuffer representing state of all keys

  htable_t *keymap;                     // Keymap for key-transformations, optional
  char *mapping_lang;                   // Mapping language to apply, optional
} keyboard_animation_t;

/**
 * @brief Load the animation table from a file
 * 
 * @param floc File location, full absolute path ending in .ini
 * @param err Error output buffer, set to NULL if unused
 * @return htable_t* Loaded table or NULL on errors
 */
keyboard_animation_t *keyboard_animation_load(const char *floc, char **err);

/**
 * @brief Play a pre-loaded keyboard animation on a specified keyboard device
 * 
 * @param animation Animation to play
 * @param kb Keyboard to play on
 * @param err Error string buffer, leave at NULL if not needed
 * 
 * @return true Frame sent to device successfully
 * @return false Could not send frame to device or frame out of range
 */
bool keyboard_animation_dispatch_frame(
  keyboard_animation_t *animation,
  keyboard_t *kb,
  char **err
);

/**
 * @brief Launch a previously loaded animation in a loop on a certain device
 * 
 * @param animation Animation to launch
 * @param keymap Keymap to be used by the animation, NULLable
 * @param kb Keyboard device to execute on
 * 
 * @returns true On successful thread instantiation
 * @returns false On internal errors
 */
bool keyboard_animation_launch(
  keyboard_animation_t *animation,
  htable_t *keymap,
  keyboard_t *kb
);

/**
 * @brief Quit the animation if it's currently looping, do nothing otherwise
 * 
 * @param animation Animation to quit
 */
void keyboard_animation_quit(keyboard_animation_t *animation);

#endif