#ifndef iniparse_h
#define iniparse_h

#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "util/htable.h"
#include "keyboard_key.h"
#include "util/strtrim.h"
#include "util/strind.h"
#include "util/partial_strdup.h"

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
 * @param max_secs Maximum number of sections
 * @param max_keys_per_sec Maximum number of keys per section
 * @return htable_t* Parsed table, NULL on errors, errno will be set
 */
htable_t *iniparse_read(const char *floc, char **err, size_t max_secs, size_t max_keys_per_sec);

/**
 * @brief Write a ini from memory into a file
 * 
 * @param ini Ini in memory
 * @param floc File location
 * @param err Error string output buffer, NULL if not needed
 * 
 * @return true File written successfully
 * @return false Could not write the file
 */
bool iniparse_write(htable_t *ini, const char *floc, char **err);

/**
 * @brief Dump a parsed keymap
 * 
 * @param keymap Keymap to dump
 */
char *iniparse_dump(htable_t *keymap);

#endif