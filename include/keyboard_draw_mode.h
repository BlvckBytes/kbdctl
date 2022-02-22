#ifndef keyboard_draw_mode_h
#define keyboard_draw_mode_h

#include "util/enumlut.h"

// Keyboard key frame drawing mode and their corresponding values
#define _EVALS_KEYBOARD_DRAW_MODE(FUN)                                                      \
  FUN(KDM_RESET_BEFORE,   0x00) /* Reset before drawing the current frame */                \
  FUN(KDM_ADD_PREV,       0x01) /* Set the state of the last frame before proceeding */     \
  FUN(KDM_ADD_ALL,        0x02) /* All frames manipulate a shared state table */

ENUM_TYPEDEF_FULL_IMPL(keyboard_draw_mode, _EVALS_KEYBOARD_DRAW_MODE);

#endif