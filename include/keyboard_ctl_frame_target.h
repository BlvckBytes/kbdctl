#ifndef keyboard_ctl_frame_target_h
#define keyboard_ctl_frame_target_h

#include "util/enumlut.h"

/**
 * @brief Where will the change be applied to?
 */
#define _EVALS_KEYBOARD_CTL_FRAME_TARGET(FUN)                       \
  FUN(TARG_KEYS, 0x00)  /* This frame will change key lighting */   \
  FUN(TARG_LOGO, 0x01)  /* This frame will change logo lighting */

ENUM_TYPEDEF_FULL_IMPL(keyboard_ctl_frame_target, _EVALS_KEYBOARD_CTL_FRAME_TARGET);

#endif