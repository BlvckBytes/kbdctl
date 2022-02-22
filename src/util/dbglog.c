#include "util/dbglog.h"

void dbgerr(const char *fmt, ...)
{
  #ifdef DEBUG_ERR
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
  #endif
}

void dbginf(const char *fmt, ...)
{
  #ifdef DEBUG_INF
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stdout, fmt, ap);
  va_end(ap);
  #endif
}