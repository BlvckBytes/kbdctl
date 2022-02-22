#ifndef strclone_h
#define strclone_h

#include <stddef.h>
#include <string.h>

#include "util/quickmaths.h"
#include "util/mman.h"

/**
 * @brief Clone a string and limit it's length, returns NULL if the string was too large
 * 
 * @param origin Original string to be cloned
 * @param max_len Maximum length of the clone
 * @return char* Cloned string, NULL on errors
 */
char *strclone(const char *origin, size_t max_len);

#endif