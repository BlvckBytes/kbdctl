#ifndef dbglog_h
#define dbglog_h

#include <stdio.h>
#include <stdarg.h>

// Debug modes
#define DEBUG_ERR
#define DEBUG_INF

/**
 * @brief Debug log error messages
 */
void dbgerr(const char *fmt, ...);

/**
 * @brief Debug log information messages
 */
void dbginf(const char *fmt, ...);

#endif