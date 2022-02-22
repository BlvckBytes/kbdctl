#ifndef keyboard_animation_h
#define keyboard_animation_h

#include <unistd.h>

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
  long frame_del;                       // Delay in milliseconds between frames
  long last_frame;                      // Last frame specified without gaps
  keyboard_draw_mode_t draw_mode;       // Draw mode to use for data-management between frames
  char *mapping_lang;                   // Mapping language to apply
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
 * @param keymap Keymap for remapping keys, optional and NULLable
 * @param kb Keyboard to play on
 * @param curr_frame External current frame index tracker
 * @param framebuf External framebuffer, leave at NULL for auto-initialization
 * @param err Error string buffer, leave at NULL if not needed
 * 
 * @return true Frame sent to device successfully
 * @return false Could not send frame to device or frame out of range
 */
bool keyboard_animation_play(
  keyboard_animation_t *animation,
  htable_t *keymap,
  keyboard_t *kb,
  size_t curr_frame,
  dynarr_t **framebuf,
  char **err
);

#endif