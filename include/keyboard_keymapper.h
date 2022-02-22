#ifndef keyboard_keymapper_h
#define keyboard_keymapper_h

#include "keyboard_key.h"
#include "util/htable.h"
#include "util/iniparse.h"

/**
 * @brief Load the keymapping table from a file
 * 
 * @param floc File location, full absolute path ending in .ini
 * @param err Error output buffer, set to NULL if unused
 * @return htable_t* Loaded table or NULL on errors
 */
htable_t *keyboard_keymapper_load(const char *floc, char **err);

/**
 * @brief Safely look up a key's map-target based on a given language. This
 * never fails and just returns the input key itself on errors (no mapping)
 * 
 * @param keymap Keymap that has been loaded before
 * @param lang Language to look in
 * @param key Key to look for
 * @return keyboard_key_t Mapped key
 */
keyboard_key_t keyboard_keymapper_lookup(htable_t *keymap, const char *lang, keyboard_key_t key);

#endif