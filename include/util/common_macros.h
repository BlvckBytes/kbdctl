#ifndef common_macros_h
#define common_macros_h

// Debug mode
#define DEBUG_EN

// Marks an always inlined function
#define INLINED __attribute__((always_inline)) inline

// Carriage return and line feed
#define CRLF "\r\n"

// String wrapped in quotes used in combination with printf
#define QUOTSTR "\"%s\""

#endif