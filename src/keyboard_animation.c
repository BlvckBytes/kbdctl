#include "keyboard_animation.h"

static void keyboard_animation_cleanup(mman_meta_t *meta)
{
  keyboard_animation_t *anim = ((keyboard_animation_t *) meta->ptr);

  // Dealloc the parsed ini
  mman_dealloc(anim->animation);
}

keyboard_animation_t *keyboard_animation_load(const char *floc, char **err)
{
  // Try to parse the ini file
  scptr htable_t *ini = iniparse(floc, err, 512, 256);
  if (!ini) return NULL;

  // Create animation wrapper
  scptr keyboard_animation_t *anim = (keyboard_animation_t *) mman_alloc(sizeof(keyboard_animation_t), 1, keyboard_animation_cleanup);
  anim->animation = mman_ref(ini);

  // Define fallback values
  anim->frame_del = 200;
  anim->draw_mode = KDM_RESET_BEFORE;
  anim->last_frame = 0;

  // Look for the last frame specified starting at 1 without any gaps
  for (size_t i = 1; i < SIZE_MAX; i++)
  {
    // Stop on missing frame section
    scptr char *i_str = strfmt_direct("%lu", i);
    if (!htable_contains(ini, i_str)) break;
    // Set the last frame to the currently reached section
    anim->last_frame = i;
  }

  // Try to load the settings section
  htable_t *settings = NULL;
  if (htable_fetch(ini, "settings", &settings) != HTABLE_SUCCESS)
    return mman_ref(anim);

  // Load frame delay if defined
  char *frame_del_str = NULL;
  if (htable_fetch(settings, "frame_del", (void**) &frame_del_str) == HTABLE_SUCCESS)
  {
    // Try to parse the string as a long and update the value on success
    long frame_del = 0;
    if (longp(&frame_del, frame_del_str, 10) == LONGP_SUCCESS)
      anim->frame_del = frame_del;
  }

  // Load draw mode if defined
  char *draw_mode_str = NULL;
  if (htable_fetch(settings, "draw_mode", (void**) &draw_mode_str) == HTABLE_SUCCESS)
  {
    // Try to parse the numeric enum value and update the value on success
    keyboard_draw_mode_t draw_mode;
    if (keyboard_draw_mode_value(draw_mode_str, &draw_mode) == ENUMLUT_SUCCESS)
      anim->draw_mode = draw_mode;
  }

  return mman_ref(anim);
}