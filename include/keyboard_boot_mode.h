#ifndef keyboard_boot_mode_h
#define keyboard_boot_mode_h

#include <stddef.h>
#include <string.h>
#include "util/enumlut.h"

// Boot modes and their corresponding values
#define _EVALS_KEYBOARD_BOOT_MODE(FUN)                                    \
        FUN(BOOT_STORAGE, 0x01) /* Boot into the user-defined storage */  \
        FUN(BOOT_FACTORY, 0x02) /* Boot into the mode it shipped with */

ENUM_TYPEDEF_FULL_IMPL(keyboard_boot_mode, _EVALS_KEYBOARD_BOOT_MODE);

#endif