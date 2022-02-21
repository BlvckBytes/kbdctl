#ifndef keymap_parser_h
#define keymap_parser_h

#include <stdlib.h>

#include "htable.h"
#include "keyboard_key.h"
#include "strtrim.h"
#include "strind.h"
#include "partial_strdup.h"

#define KEYMAP_PARSER_MAX_LANGS 32
#define KEYMAP_PARSER_MAX_KEYS 256

// Initial length of the string buffer used when printing key mappings
#define KEYMAP_PARSER_MAPPINGS_PRINT_INIT_LEN 512

// Initial length of the string buffer used when errors occur
#define KEYMAP_PARSER_ERROR_INIT_LEN 128

/**
 * @brief Parse the keymap configuration into a hashtable with two levels:
 * Outer table: key=language, value=remap-table
 * Inner table: key=from_key, value=to_key
 * 
 * @param floc File location, full absolute path, ending in .ini
 * @param err Error string output buffer, NULL if not needed
 * @return htable_t* Parsed table, NULL on errors, errno will be set
 */
htable_t *keymap_parser_parse(const char *floc, char **err);

/**
 * @brief Print a parsed keymap to stdout for debugging purposes
 * 
 * @param keymap Keymap to print
 */
void keymap_parser_print(htable_t *keymap);

#endif