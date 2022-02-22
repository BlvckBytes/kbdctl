#ifndef keyboard_animaton_h
#define keyboard_animaton_h

#include "keyboard_draw_mode.h"
#include "util/iniparse.h"
#include "util/htable.h"
#include "util/longp.h"

typedef struct keyboard_animation
{
  htable_t *animation;
  long frame_del;
  keyboard_draw_mode_t draw_mode;
} keyboard_animation_t;

/**
 * @brief Load the animation table from a file
 * 
 * @param floc File location, full absolute path ending in .ini
 * @param err Error output buffer, set to NULL if unused
 * @return htable_t* Loaded table or NULL on errors
 */
keyboard_animation_t *keyboard_animaton_load(const char *floc, char **err);

#endif