#include "keymap_parser.h"

#define keymap_parser_err(fmt, ...) \
  { \
    *err = mman_alloc(sizeof(char), KEYMAP_PARSER_ERROR_INIT_LEN, NULL); \
    strfmt(err, NULL, fmt, ##__VA_ARGS__); \
    return NULL; \
  }

htable_t *keymap_parser_parse(const char *floc, char **err)
{
  // Allocate outer "language section" table
  scptr htable_t *langs = htable_make(KEYMAP_PARSER_MAX_LANGS, mman_dealloc);

  // Open keymap file
  FILE *f = fopen(floc, "r");
  if (!f) return NULL;

  // Reading utility buffers
  size_t read_len, buf_len, line_ind = 0;
  char *line = NULL;

  htable_t *curr_lang = NULL;

  // Read this file line by line
  while ((read_len = getline(&line, &buf_len, f)) != -1) {
    line_ind++;

    // Comment-only line
    if (line[0] == ';') continue;

    // Strip this line of a trailing comment, if applicable
    long line_commind = strind(line, ";", 0);
    scptr char *line_nocomm = substr(line, 0, line_commind > 0 ? line_commind - 1 : line_commind);

    // Trim the comment-stripped line to make the final sanitized line
    scptr char *san_line = strtrim(line_nocomm);

    // Empty line
    if (san_line == NULL) continue;

    // Check if this is a section-begin-marker
    size_t san_line_len = strlen(san_line);
    if (san_line[0] == '[' && san_line[san_line_len - 1] == ']')
    {
      scptr char *lang = substr(san_line, 1, san_line_len - 2);

      // Invalid section begin occurred
      if (lang == NULL)
        keymap_parser_err("Invalid section-begin occurred in line %lu!", line_ind);

      // This section's language is already known
      if (htable_contains(langs, lang))
        htable_fetch(langs, lang, (void **) &curr_lang);
      
      // Create new language
      else
      {
        scptr htable_t *mappings = htable_make(KEYMAP_PARSER_MAX_KEYS, mman_dealloc);
        htable_insert(langs, lang, mman_ref(mappings));
        curr_lang = mappings;
      }

      continue;
    }

    if (!curr_lang)
      keymap_parser_err("Found keys before section-start in line %lu!", line_ind);

    // Split on =
    size_t kv_offs = 0;
    scptr char *key = partial_strdup(san_line, &kv_offs, "=", false);
    scptr char *value = partial_strdup(san_line, &kv_offs, "\0", false);

    // Insert kv pair
    htable_insert(curr_lang, key, mman_ref(value));
  }

  // Free linebuffer alloced by getline, if applicable
  // Also close the file handle
  if (line) free(line);
  fclose(f);

  return mman_ref(langs);
}

static char *keymap_parser_stringify_mappings(void *item)
{
  htable_t *mappings = (htable_t *) item;

  // Allocate resulting string buffer
  scptr char *res = (char *) mman_alloc(sizeof(char), KEYMAP_PARSER_MAPPINGS_PRINT_INIT_LEN, NULL);

  // List all keys (remappings)
  scptr char **keys = NULL;
  htable_list_keys(mappings, &keys);

  // Iterate keys
  size_t res_offs = 0;
  for (char **key = keys; *key; key++)
  {
    // Get the mapping destination
    char *value;
    htable_fetch(mappings, *key, &value);

    // Print k=v pairs with comma separators
    strfmt(&res, &res_offs, "%s%s=%s", key == keys ? "" : ", ", *key, value);
  }

  return mman_ref(res);
}

void keymap_parser_print(htable_t *keymap)
{
  printf("Parsed keymap:\n");
  scptr char *dump = htable_dump_hr(keymap, keymap_parser_stringify_mappings);
  printf("%s", dump);
}