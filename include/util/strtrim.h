#ifndef strtrim_h
#define strtrim_h

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "util/mman.h"
#include "util/substr.h"

/**
 * @brief Get a trimmed copy of a string
 * 
 * @param str Untrimmed input string
 * @return char* Trimmed copy
 */
char *strtrim(const char *str);

/**
 * @brief Get a r-trimmed copy of a string
 * 
 * @param str Untrimmed input string
 * @return char* R-Trimmed copy
 */
char *strrtrim(const char *str);

/**
 * @brief Get a l-trimmed copy of a string
 * 
 * @param str Untrimmed input string
 * @return char* L-Trimmed copy
 */
char *strltrim(const char *str);

#endif