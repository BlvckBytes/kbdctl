#include "util/iniparse.h"

#define iniparse_err(fmt, ...) \
  { \
    *err = mman_alloc(sizeof(char), INIPARSE_ERROR_INIT_LEN, NULL); \
    strfmt(err, NULL, fmt, ##__VA_ARGS__); \
    return NULL; \
  }

htable_t *iniparse(const char *floc, char **err)
{
  // Allocate outer section table
  scptr htable_t *secs = htable_make(INIPARSE_MAX_SECS, mman_dealloc_nr);

  // Open keymap file
  FILE *f = fopen(floc, "r");
  if (!f) return NULL;

  // Reading utility buffers
  size_t read_len, buf_len, line_ind = 0;
  char *line = NULL;

  // Section currently in
  // This will change over time as the file is scanned from top to bottom
  htable_t *curr_sec = NULL;

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
      scptr char *sec = substr(san_line, 1, san_line_len - 2);

      // Invalid section begin occurred
      if (sec == NULL)
        iniparse_err("Invalid section-begin occurred in line %lu!", line_ind);

      // This section's name is already known
      if (htable_contains(secs, sec))
        htable_fetch(secs, sec, (void **) &curr_sec);
      
      // Create new section
      else
      {
        scptr htable_t *mappings = htable_make(INIPARSE_MAX_KEYS, mman_dealloc_nr);
        htable_insert(secs, sec, mman_ref(mappings));
        curr_sec = mappings;
      }

      // Next iterations will place their k=v pairs into the current section
      continue;
    }

    // Not in a section but already at a key
    if (!curr_sec)
      iniparse_err("Found keys before section-start in line %lu!", line_ind);

    // Split on =
    size_t kv_offs = 0;
    scptr char *key = partial_strdup(san_line, &kv_offs, "=", false);
    scptr char *value = partial_strdup(san_line, &kv_offs, "\0", false);

    // Insert kv pair
    htable_insert(curr_sec, key, mman_ref(value));
  }

  // Free linebuffer alloced by getline, if applicable
  // Also close the file handle
  if (line) free(line);
  fclose(f);

  return mman_ref(secs);
}

static char *iniparse_stringify_kv(void *item)
{
  htable_t *pairs = (htable_t *) item;

  // Allocate resulting string buffer
  scptr char *res = (char *) mman_alloc(sizeof(char), INIPARSE_MAPPINGS_PRINT_INIT_LEN, NULL);

  // List all keys (remappings)
  scptr char **keys = NULL;
  htable_list_keys(pairs, &keys);

  // Iterate keys
  size_t res_offs = 0;
  for (char **key = keys; *key; key++)
  {
    // Get the mapping destination
    char *value = NULL;
    htable_fetch(pairs, *key, (void **) &value);

    // Print k=v pairs with comma separators
    strfmt(&res, &res_offs, "%s%s=%s", key == keys ? "" : ", ", *key, value);
  }

  return mman_ref(res);
}

char *iniparse_dump(htable_t *keymap)
{
  scptr char *dump = htable_dump_hr(keymap, iniparse_stringify_kv);
  return mman_ref(dump);
}