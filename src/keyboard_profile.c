#include "keyboard_profile.h"

ENUM_LUT_FULL_IMPL(keyboard_profile_key_lighting_type, _EVALS_KEYBOARD_PROFILE_KEY_LIGHTING_TYPE);

/*
============================================================================
                             Private routines                               
============================================================================
*/

/**
 * @brief Build the file path from a given directory and profile name
 * 
 * @param dir Directory to store into
 * @param name Profile name without file ending
 * @return char* Path used for r/W
 */
INLINED static char *keyboard_profile_build_path(const char *dir, const char *name)
{
  return strfmt_direct("%s/%s.ini", dir, name);
}

/**
 * @brief Convert a keyboard profile effect's values into a hash table
 * 
 * @param key_pref Prefix of the individual keys, unused = ""
 * @param eff Profile effect to convert
 * @return htable_t* Table with all necessary k/v pairs
 */
INLINED static htable_t *keyboard_profile_effect_to_table(const char *key_pref, keyboard_profile_effect_t eff)
{
  scptr htable_t *res = htable_make(3, mman_dealloc_nr);

  const char *effect_name = keyboard_effect_name(eff.effect);

  scptr char *name_k = strfmt_direct("%sname", key_pref);
  htable_insert(res, name_k, strclone(effect_name));

  scptr char *color_k = strfmt_direct("%scolor", key_pref);
  htable_insert(res, color_k, keyboard_color_to_hex(&(eff.color)));

  scptr char *time_k = strfmt_direct("%stime", key_pref);
  htable_insert(res, time_k, strfmt_direct("%lu", eff.time));

  return mman_ref(res);
}

static void keyboard_profile_cleanup(mman_meta_t *meta)
{
  keyboard_profile_t *prof = (keyboard_profile_t *) meta->ptr;

  // Dealloc keymap string
  mman_dealloc(prof->keymap_lang);

  // Decide what key_lighting needs
  switch(prof->key_lighting_type)
  {
    // Dealloc animation name string
    case KBPKLT_ANIM:
      mman_dealloc(prof->key_lighting.animation_name);
      break;

    // Dealloc custom key hash table
    case KBPKLT_CUSTOM:
      mman_dealloc(prof->key_lighting.custom_keys);
      break;

    // No dealloc required
    default:
      break;
  }
}

/*
============================================================================
                             Public routines                                
============================================================================
*/

keyboard_profile_t *keyboard_profile_make()
{
  scptr keyboard_profile_t *res = mman_alloc(sizeof(keyboard_profile_t), 1, keyboard_profile_cleanup);

  // White color shorthand
  keyboard_color_t _white = { 0xFF, 0xFF, 0xFF };

  // Static white effect shorthand
  keyboard_profile_effect_t _static_white = (keyboard_profile_effect_t) {
    .effect = EFFECT_COLOR,
    .color  = _white,
    .time   = 0
  };

  // Set default values
  res->boot_mode = BOOT_FACTORY;                             // Default boot mode
  res->keymap_lang = NULL;                                   // No mappings
  res->status_color = _white;                                // White status
  res->effect_logo = _static_white;                          // Static white logo
  res->key_lighting_type = KBPKLT_EFFECT;                    // Keys are lit using an effect
  res->key_lighting = (keyboard_profile_key_lighting_t) {    // Static white keys
    .effect_keys = _static_white
  };

  return mman_ref(res);
}

bool keyboard_profile_save(const char *dir, const char *name, keyboard_profile_t *profile, char **err)
{
  scptr char *path = keyboard_profile_build_path(dir, name);
  scptr FILE **f = (FILE **) mman_wrap(fopen(path, "w"), (clfn_t) fclose);
  fputs("to be implemented", *f);
  return false;
}

keyboard_profile_t *keyboard_profile_load(const char *dir, const char *name, char **err)
{
  return NULL;
}

htable_t *keyboard_profile_to_ini(keyboard_profile_t *profile)
{
  scptr htable_t *ini = htable_make(128, mman_dealloc_nr);

  // Key lighting
  scptr htable_t *key_lighting = htable_make(keyboard_key_length() + 1, mman_dealloc_nr);
  htable_insert(ini, "key_lighting", key_lighting);

  const char *key_lighting_type = keyboard_profile_key_lighting_type_name(profile->key_lighting_type);
  keyboard_profile_key_lighting_t klt = profile->key_lighting;

  htable_insert(key_lighting, "key_lighting_type", strclone(key_lighting_type));
  switch(profile->key_lighting_type)
  {
  // Write animation name
  case KBPKLT_ANIM:
  {
    htable_insert(key_lighting, "animation_name", strclone(klt.animation_name));
    break;
  }

  // Write effect name, color and time
  case KBPKLT_EFFECT:
  {
    scptr htable_t *efft = keyboard_profile_effect_to_table("effect_", klt.effect_keys);
    htable_append_table(key_lighting, efft, HTABLE_AM_OVERRIDE, (htable_value_clone_f) strclone);
    break;
  }

  // Write custom keymap
  case KBPKLT_CUSTOM:
  {
    scptr char **ckeys = NULL;
    htable_list_keys(klt.custom_keys, &ckeys);

    for (char **ckey = ckeys; *ckey; ckey++)
    {
      keyboard_color_t *ckey_col = NULL;
      if (htable_fetch(klt.custom_keys, *ckey, (void **) &ckey_col) != HTABLE_SUCCESS)
        continue;

      htable_insert(key_lighting, *ckey, keyboard_color_to_hex(ckey_col));
    }
    break;
  }
  }

  // Logo effect
  scptr htable_t *logo_effect = htable_make(16, mman_dealloc_nr);
  htable_insert(ini, "logo_effect", logo_effect);
  scptr htable_t *logo_efft = keyboard_profile_effect_to_table("", profile->effect_logo);
  htable_append_table(logo_effect, logo_efft, HTABLE_AM_OVERRIDE, (htable_value_clone_f) strclone);

  // Configuration (one-value fields)
  scptr htable_t *config = htable_make(1, mman_dealloc_nr);
  htable_insert(ini, "config", config);
  htable_insert(config, "boot_mode", (void *) keyboard_boot_mode_name(profile->boot_mode));
  htable_insert(config, "status_color", keyboard_color_to_hex(&(profile->status_color)));

  if (profile->keymap_lang)
    htable_insert(config, "keymap_lang", strclone(profile->keymap_lang));

  return mman_ref(ini);
}

keyboard_profile_t *keyboard_profile_from_ini(htable_t *ini)
{
  return NULL;
}