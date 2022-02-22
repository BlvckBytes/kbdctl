#include "keyboard_keymapper.h"

htable_t *keyboard_keymapper_load(const char *floc, char **err)
{
  scptr htable_t *res = iniparse(floc, err);

  if (!res) return NULL;
  return mman_ref(res);
}

keyboard_key_t keyboard_keymapper_lookup(htable_t *keymap, const char *lang, keyboard_key_t key)
{
  // Language unknown, don't map
  if (!htable_contains(keymap, lang))
    return key;

  // Get language section, don't map on errors
  htable_t *lang_sec;
  if (htable_fetch(keymap, lang, (void **) &lang_sec) != HTABLE_SUCCESS)
    return key;

  // Get the key's remapping target, don't map on errors
  char *tar_key;
  if(htable_fetch(lang_sec, keyboard_key_name(key), (void **) &tar_key) != HTABLE_SUCCESS)
    return key;

  // Return the mapping result
  return keyboard_key_value(tar_key);
}