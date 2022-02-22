#include "keyboard_animation.h"

#define kbanim_err(fmt, ...)                 \
  {                                            \
    *err = strfmt_direct(fmt, ##__VA_ARGS__);  \
    return false;                              \
  }

static void keyboard_animation_cleanup(mman_meta_t *meta)
{
  keyboard_animation_t *anim = ((keyboard_animation_t *) meta->ptr);

  // Dealloc the parsed ini and the mapping language string
  mman_dealloc(anim->ini);
  mman_dealloc(anim->mapping_lang);
}

keyboard_animation_t *keyboard_animation_load(const char *floc, char **err)
{
  // Try to parse the ini file
  scptr htable_t *ini = iniparse(floc, err, 512, 256);
  if (!ini) return NULL;

  // Create animation wrapper
  scptr keyboard_animation_t *anim = (keyboard_animation_t *) mman_alloc(sizeof(keyboard_animation_t), 1, keyboard_animation_cleanup);
  anim->ini = mman_ref(ini);

  // Define fallback values
  anim->frame_del = 200;
  anim->draw_mode = KDM_RESET_BEFORE;
  anim->last_frame = 0;
  anim->mapping_lang = NULL;

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
  if (htable_fetch(ini, "settings", (void **) &settings) != HTABLE_SUCCESS)
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

  // Load mapping language if defined
  htable_fetch(settings, "mapping_lang", (void **) &(anim->mapping_lang));

  return mman_ref(anim);
}

/**
 * @brief Clear all available keys by setting their color to #000000
 * 
 * @param framebuf Framebuffer to clear the keys in
 */
INLINED static void keyboard_animation_clear_keys(dynarr_t **framebuf)
{
  // TODO: Hardcoding start- and endkeys here is not optimal...
  for (size_t i = KEY_A; i <= KEY_WIN_RIGHT; i++)
  {
    keyboard_color_t rgb_color = { 0x00, 0x00, 0x00 };
    scptr keyboard_key_color_t *key_color = keyboard_key_color_make(i, rgb_color);
    dynarr_set_at(*framebuf, i, mman_ref(key_color));

    if (i == KEY_MENU) i = KEY_CTRL_LEFT - 1; // Jump gap
  }
}

bool keyboard_animation_play(
  keyboard_animation_t *animation,
  htable_t *keymap,
  keyboard_t *kb,
  size_t curr_frame,
  dynarr_t **framebuf,
  char **err
)
{
  // Check frame index validity
  if (curr_frame > animation->last_frame)
    kbanim_err("Frame index out of range! (%lu)", curr_frame);

  // Initialize a framebuffer if it's not yet existing
  if (*framebuf == NULL)
    *framebuf = dynarr_make(256, 256, mman_dealloc_nr);
  
  if (
    // Clear the framebuffer if resetting before each frame is desired
    animation->draw_mode == KDM_RESET_BEFORE

    // Only keep the previous state, clear the framebuffer otherwise
    // This basically means clearing on non-even frame indices (starting on 1)
    // Clearing: 1 3 5 ...
    || (animation->draw_mode == KDM_ADD_PREV && curr_frame % 2 != 0)

    // Start out with a cleared buffer on the first frame
    || curr_frame == 1
  )
    keyboard_animation_clear_keys(framebuf);

  // Get frame contents
  scptr char *frame_sect = strfmt_direct("%lu", curr_frame);
  htable_t *frame = NULL;
  if (htable_fetch(animation->ini, frame_sect, (void **) &frame) != HTABLE_SUCCESS)
    kbanim_err("Frame section not found! (" QUOTSTR ")", frame_sect);

  // List all mappings
  scptr char **keys = NULL;
  htable_list_keys(frame, &keys);

  // Iterate all mappings
  // Key = Key enum name, Value = RGB hex
  for (char **key = keys; *key; key++)
  {
    // Just skip invalid keys
    keyboard_key_t key_val;
    if (keyboard_key_value(*key, &key_val) != ENUMLUT_SUCCESS)
    {
      dbgerr("Invalid key in animation-frame %lu: %s\n", curr_frame, *key);
      continue;
    }

    // Remap this key, if a map and a language has been provided
    if (animation->mapping_lang && keymap)
      key_val = keyboard_keymapper_lookup(keymap, animation->mapping_lang, key_val);

    char *rgb_str = NULL;
    if (htable_fetch(frame, *key, (void **) &rgb_str) != HTABLE_SUCCESS)
      kbanim_err("Could not fetch key entry! (" QUOTSTR ")", *key);
    
    keyboard_color_t rgb_color = { 0x00, 0x00, 0x00 };

    // Try to parse the rgb value from base 16, fallback to 0 on malformed values
    long rgb_val = 0;
    if (longp(&rgb_val, rgb_str, 16) == LONGP_SUCCESS)
    {
      // R  G  B
      // XX XX XX
      rgb_color.r = (rgb_val >> 16) & 0xFF;
      rgb_color.g = (rgb_val >> 8)  & 0xFF;
      rgb_color.b = (rgb_val >> 0)  & 0xFF;
    }
    else
      dbgerr("Invalid color in animation-frame %lu: %s\n", curr_frame, rgb_str);

    // Try to set the key within the framebuffer
    scptr keyboard_key_color_t *key_color = keyboard_key_color_make(key_val, rgb_color);
    if (dynarr_set_at(*framebuf, key_val, mman_ref(key_color)) != dynarr_SUCCESS)
      kbanim_err("Could not set a key at slot %lu in the framebuffer!", key_val);
  }

  // Get current key array state
  scptr keyboard_key_color_t **key_arr = NULL;
  size_t num_keys = dynarr_as_array(*framebuf, (void ***) &key_arr);

  // Make items frame
  scptr uint8_t *data_keys = keyboard_ctl_frame_make(TYPE_KEYS);

  // Append all keys and send, may take multiple frames as one frame has limited capacity
  size_t keys_offs = 0;
  while (num_keys != keys_offs)
  {
    keyboard_ctl_frame_key_list_apply(data_keys, key_arr, num_keys, KGA_KEY, &keys_offs);
    if (!keyboard_transmit(kb, data_keys, mman_fetch_meta(data_keys)->num_blocks))
      kbanim_err("Could not transmit (partial) frame data!");

    // Small delay between control frames
    usleep(800);
  }

  // Commit changes and thus make them visible
  scptr uint8_t *data_comm = keyboard_ctl_frame_make(TYPE_COMMIT);
  if (!keyboard_transmit(kb, data_comm, mman_fetch_meta(data_comm)->num_blocks))
    kbanim_err("Could not transmit commit data!");

  // Successfully applied frame
  return true;
}