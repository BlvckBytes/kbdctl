#include "util/iniparse.h"

#define iniparse_readerr(fmt, ...)             \
  {                                            \
    *err = strfmt_direct(fmt, ##__VA_ARGS__);  \
    return NULL;                               \
  }

#define iniparse_writeerr(fmt, ...)            \
  {                                            \
    *err = strfmt_direct(fmt, ##__VA_ARGS__);  \
    return false;                              \
  }

htable_t *iniparse_read(const char *floc, char **err, size_t max_secs, size_t max_keys_per_sec)
{
  // Allocate outer section table
  scptr htable_t *secs = htable_make(max_secs, mman_dealloc_nr);

  // Open keymap file
  scptr FILE **f = (FILE **) mman_wrap(fopen(floc, "r"), (clfn_t) fclose);

  // Could not open the file
  if (!*f)
    iniparse_readerr("Could not open the file " QUOTSTR " (" QUOTSTR ")!", floc, strerror(errno));

  // Reading utility buffers
  size_t read_len, buf_len, line_ind = 0;
  scptr char **line = (char **) mman_wrap(NULL, free);

  // Section currently in
  // This will change over time as the file is scanned from top to bottom
  htable_t *curr_sec = NULL;

  // Read this file line by line
  while ((read_len = getline(line, &buf_len, *f)) != -1) {
    line_ind++;

    // Comment-only line
    if ((*line)[0] == ';') continue;

    // Strip this line of a trailing comment, if applicable
    long line_commind = strind(*line, ";", 0);
    scptr char *line_nocomm = substr(*line, 0, line_commind > 0 ? line_commind - 1 : line_commind);

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
        iniparse_readerr("Invalid section-begin occurred in line %lu!", line_ind);

      // This section's name is already known
      if (htable_contains(secs, sec))
        htable_fetch(secs, sec, (void **) &curr_sec);
      
      // Create new section
      else
      {
        scptr htable_t *mappings = htable_make(max_keys_per_sec, mman_dealloc_nr);
        htable_insert(secs, sec, mman_ref(mappings));
        curr_sec = mappings;
      }

      // Next iterations will place their k=v pairs into the current section
      continue;
    }

    // Not in a section but already at a key
    if (!curr_sec)
      iniparse_readerr("Found keys before section-start in line %lu!", line_ind);

    // Split on =
    size_t kv_offs = 0;
    scptr char *key = partial_strdup(san_line, &kv_offs, "=", false);
    scptr char *value = partial_strdup(san_line, &kv_offs, "\0", false);

    // Insert kv pair
    htable_insert(curr_sec, key, mman_ref(value));
  }

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

bool iniparse_write(htable_t *ini, const char *floc, char **err)
{
  // Open output file
  scptr FILE **f = (FILE **) mman_wrap(fopen(floc, "w"), (clfn_t) fclose);

  // Could not open the file
  if (!*f)
    iniparse_writeerr("Could not open the file " QUOTSTR " (" QUOTSTR ")!", floc, strerror(errno));

  // Loop all section titles
  scptr char **sections = NULL;
  htable_list_keys(ini, &sections);
  for (char **section = sections; *section; section++)
  {
    // Get section
    scptr htable_t *sectable = NULL;
    htable_fetch(ini, *section, (void **) &sectable);

    // Write section header, put a leading newline on !first line
    scptr char *secthead = strfmt_direct("%s[%s]\n", section == sections ? "" : "\n", *section);
    fputs(secthead, *f);

    // Loop all keys of that section
    scptr char **keys = NULL;
    htable_list_keys(sectable, &keys);
    for (char **key = keys; *key; key++)
    {
      // Get value to this key
      char *value = NULL;
      htable_fetch(sectable, *key, (void **) &value);

      // Write key-value pair
      scptr char *line = strfmt_direct("%s=%s\n", *key, value);
      fputs(line, *f);
    }
  }

  // Success
  return true;
}

char *iniparse_dump(htable_t *keymap)
{
  scptr char *dump = htable_dump_hr(keymap, iniparse_stringify_kv);
  return mman_ref(dump);
}