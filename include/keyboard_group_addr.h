#ifndef keyboard_group_addr_h
#define keyboard_group_addr_h

#include "util/enumlut.h"

// The group's address a keyboard item is residing in
#define _EVALS_KEYBOARD_GROUP_ADDR(FUN) \
        FUN(KGA_STATUS,     0x4005)     \
        FUN(KGA_KEY,        0x010E)

ENUM_TYPEDEF_FULL_IMPL(keyboard_group_addr, _EVALS_KEYBOARD_GROUP_ADDR);

#endif