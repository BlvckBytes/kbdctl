#ifndef strconv_h
#define strconv_h

#include <wchar.h>
#include "mman.h"

/**
 * @brief Convert a wide-char string to a standard c string
 * 
 * @param str Wide-char string to convert
 * @param max_len Maximum length that string can have
 * @return char* Converted result
 */
char *strconv(const wchar_t *str, size_t max_len);

#endif