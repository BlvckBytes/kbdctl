#ifndef substr_h
#define substr_h

#include <stddef.h>
#include <string.h>

#include "mman.h"

/**
 * @brief Get a defined region as a copy from an input string
 * 
 * @param str Input string
 * @param start Start of region, zero-based
 * @param end End of region, zero-based, -1 for till' the end
 * @return char* Resulting substring
 */
char *substr(const char *str, size_t start, size_t end);

#endif