#ifndef keyboard_profile_h
#define keyboard_profile_h

#include "util/htable.h"
#include "util/enumlut.h"
#include "util/strclone.h"
#include "util/iniparse.h"
#include "keyboard_boot_mode.h"
#include "keyboard_color.h"
#include "keyboard_key.h"
#include "keyboard_effect.h"

/**
 * @brief Dictates what kind of lighting is stored in the
 * keyboard_profile_key_lighting union
 */
#define _EVALS_KEYBOARD_PROFILE_KEY_LIGHTING_TYPE(FUN)          \
  FUN(KBPKLT_EFFECT, 0x01) /* Keys have an effect applied */    \
  FUN(KBPKLT_ANIM,   0x02) /* Keys have an animation applied */ \
  FUN(KBPKLT_CUSTOM, 0x03) /* Keys have a custom map applied */

ENUM_TYPEDEF_FULL_IMPL(keyboard_profile_key_lighting_type, _EVALS_KEYBOARD_PROFILE_KEY_LIGHTING_TYPE);

/**
 * @brief Represents a built-in effect with it's parameters
 */
typedef struct keyboard_profile_effect
{
  keyboard_effect_t effect;
  size_t time;
  keyboard_color_t color;
} keyboard_profile_effect_t;

/**
 * @brief Represents the lighting applied to the keys, where only one
 * of the available types can be applied
 */
typedef union keyboard_profile_key_lighting
{
  // Keys effect
  keyboard_profile_effect_t effect_keys;

  // Animation
  char *animation_name;

  // Custom KeyColors
  htable_t *custom_keys;
} keyboard_profile_key_lighting_t;

/**
 * @brief Repersents a profile which can be stored into a file
 * and later on loaded and applied to the keyboard
 */
typedef struct keyboard_profile
{
  // Key lighting
  keyboard_profile_key_lighting_type_t key_lighting_type;
  keyboard_profile_key_lighting_t key_lighting;

  // Logo lighting
  keyboard_profile_effect_t effect_logo;

  // Boot mode
  keyboard_boot_mode_t boot_mode;

  // Status color
  keyboard_color_t status_color;

  // Keymap language
  char *keymap_lang;
} keyboard_profile_t;

/**
 * @brief Create a new profile with default values
 */
keyboard_profile_t *keyboard_profile_make();

/**
 * @brief Save a profile from memory into a file
 * 
 * @param dir Directory to write into
 * @param name Name of the profile, no file extensions
 * @param profile Profile to save
 * @param err Error output buffer
 * 
 * @return true Successfully saved
 * @return false Error, look at err
 */
bool keyboard_profile_save(const char *dir, const char *name, keyboard_profile_t *profile, char **err);

/**
 * @brief Load a profile from a file into memory
 * 
 * 
 * @param dir Directory to write into
 * @param name Name of the profile, no file extensions
 * @param err Error output buffer
 * 
 * @return keyboard_profile_t* Profile on success, NULL on error, look at err
 */
keyboard_profile_t *keyboard_profile_load(const char *dir, const char *name, char **err);

/**
 * @brief Convert a keybaord profile into an INI-style map
 * 
 * @param profile Keyboard profile to convert
 * @return htable_t* INI-style map
 */
htable_t *keyboard_profile_to_ini(keyboard_profile_t *profile);

/**
 * @brief Convert a INI-style map to a keyboard profile
 * 
 * @param ini INI-style map to convert
 * @return keyboard_profile_t* Keyboard profile
 */
keyboard_profile_t *keyboard_profile_from_ini(htable_t *ini);

#endif