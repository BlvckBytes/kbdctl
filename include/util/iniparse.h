#ifndef iniparse_h
#define iniparse_h

#include <stdlib.h>

#include "util/htable.h"
#include "keyboard_key.h"
#include "util/strtrim.h"
#include "util/strind.h"
#include "util/partial_strdup.h"

#define INIPARSE_MAX_SECS 32
#define INIPARSE_MAX_KEYS 256

// Initial length of the string buffer used when printing key mappings
#define INIPARSE_MAPPINGS_PRINT_INIT_LEN 512

// Initial length of the string buffer used when errors occur
#define INIPARSE_ERROR_INIT_LEN 128

/**
 * @brief Parse the keymap configuration into a hashtable with two levels:
 * Outer table: key=language, value=remap-table
 * Inner table: key=from_key, value=to_key
 * 
 * @param floc File location, full absolute path, ending in .ini
 * @param err Error string output buffer, NULL if not needed
 * @return htable_t* Parsed table, NULL on errors, errno will be set
 */
htable_t *iniparse(const char *floc, char **err);

/**
 * @brief Print a parsed keymap to stdout for debugging purposes
 * 
 * @param keymap Keymap to print
 */
void iniparse_print(htable_t *keymap);

#endif