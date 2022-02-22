#ifndef keyboard_draw_mode_h
#define keyboard_draw_mode_h

#include "util/enumlut.h"

// Keyboard key frame drawing mode and their corresponding values
#define _EVALS_KEYBOARD_DRAW_MODE(FUN) \
  FUN(KDM_RESET_BEFORE,   0x00)        \
  FUN(KDM_ADD_PREV,       0x01)        \
  FUN(KDM_ADD_ALL,        0x02)

ENUM_TYPEDEF_FULL_IMPL(keyboard_draw_mode, _EVALS_KEYBOARD_DRAW_MODE);

#endif