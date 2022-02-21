#include "keyboard_key.h"

/**
 * @brief Converts keyboard keys to their string representation
 */
static const char *keyboard_key_name[] = {
  [KEY_A] = "KEY_A",
  [KEY_B] = "KEY_B",
  [KEY_C] = "KEY_C",
  [KEY_D] = "KEY_D",
  [KEY_E] = "KEY_E",
  [KEY_F] = "KEY_F",
  [KEY_G] = "KEY_G",
  [KEY_H] = "KEY_H",
  [KEY_I] = "KEY_I",
  [KEY_J] = "KEY_J",
  [KEY_K] = "KEY_K",
  [KEY_L] = "KEY_L",
  [KEY_M] = "KEY_M",
  [KEY_N] = "KEY_N",
  [KEY_O] = "KEY_O",
  [KEY_P] = "KEY_P",
  [KEY_Q] = "KEY_Q",
  [KEY_R] = "KEY_R",
  [KEY_S] = "KEY_S",
  [KEY_T] = "KEY_T",
  [KEY_U] = "KEY_U",
  [KEY_V] = "KEY_V",
  [KEY_W] = "KEY_W",
  [KEY_X] = "KEY_X",
  [KEY_Y] = "KEY_Y",
  [KEY_Z] = "KEY_Z",
  [KEY_N1] = "KEY_N1",
  [KEY_N2] = "KEY_N2",
  [KEY_N3] = "KEY_N3",
  [KEY_N4] = "KEY_N4",
  [KEY_N5] = "KEY_N5",
  [KEY_N6] = "KEY_N6",
  [KEY_N7] = "KEY_N7",
  [KEY_N8] = "KEY_N8",
  [KEY_N9] = "KEY_N9",
  [KEY_N0] = "KEY_N0",
  [KEY_ENTER] = "KEY_ENTER",
  [KEY_ESC] = "KEY_ESC",
  [KEY_BACKSPACE] = "KEY_BACKSPACE",
  [KEY_TAB] = "KEY_TAB",
  [KEY_SPACE] = "KEY_SPACE",
  [KEY_MINUS] = "KEY_MINUS",
  [KEY_EQUAL] = "KEY_EQUAL",
  [KEY_OPEN_BRACKET] = "KEY_OPEN_BRACKET",
  [KEY_CLOSE_BRACKET] = "KEY_CLOSE_BRACKET",
  [KEY_BACKSLASH] = "KEY_BACKSLASH",
  [KEY_DOLLAR] = "KEY_DOLLAR",
  [KEY_SEMICOLON] = "KEY_SEMICOLON",
  [KEY_QUOTE] = "KEY_QUOTE",
  [KEY_TILDE] = "KEY_TILDE",
  [KEY_COMMA] = "KEY_COMMA",
  [KEY_PERIOD] = "KEY_PERIOD",
  [KEY_SLASH] = "KEY_SLASH",
  [KEY_CAPS_LOCK] = "KEY_CAPS_LOCK",
  [KEY_F1] = "KEY_F1",
  [KEY_F2] = "KEY_F2",
  [KEY_F3] = "KEY_F3",
  [KEY_F4] = "KEY_F4",
  [KEY_F5] = "KEY_F5",
  [KEY_F6] = "KEY_F6",
  [KEY_F7] = "KEY_F7",
  [KEY_F8] = "KEY_F8",
  [KEY_F9] = "KEY_F9",
  [KEY_F10] = "KEY_F10",
  [KEY_F11] = "KEY_F11",
  [KEY_F12] = "KEY_F12",
  [KEY_PRINT_SCREEN] = "KEY_PRINT_SCREEN",
  [KEY_SCROLL_LOCK] = "KEY_SCROLL_LOCK",
  [KEY_PAUSE_BREAK] = "KEY_PAUSE_BREAK",
  [KEY_INSERT] = "KEY_INSERT",
  [KEY_HOME] = "KEY_HOME",
  [KEY_PAGE_UP] = "KEY_PAGE_UP",
  [KEY_DEL] = "KEY_DEL",
  [KEY_END] = "KEY_END",
  [KEY_PAGE_DOWN] = "KEY_PAGE_DOWN",
  [KEY_ARROW_RIGHT] = "KEY_ARROW_RIGHT",
  [KEY_ARROW_LEFT] = "KEY_ARROW_LEFT",
  [KEY_ARROW_BOTTOM] = "KEY_ARROW_BOTTOM",
  [KEY_ARROW_TOP] = "KEY_ARROW_TOP",
  [KEY_NUM_LOCK] = "KEY_NUM_LOCK",
  [KEY_NUM_SLASH] = "KEY_NUM_SLASH",
  [KEY_NUM_ASTERISK] = "KEY_NUM_ASTERISK",
  [KEY_NUM_MINUS] = "KEY_NUM_MINUS",
  [KEY_NUM_PLUS] = "KEY_NUM_PLUS",
  [KEY_NUM_ENTER] = "KEY_NUM_ENTER",
  [KEY_NUM_1] = "KEY_NUM_1",
  [KEY_NUM_2] = "KEY_NUM_2",
  [KEY_NUM_3] = "KEY_NUM_3",
  [KEY_NUM_4] = "KEY_NUM_4",
  [KEY_NUM_5] = "KEY_NUM_5",
  [KEY_NUM_6] = "KEY_NUM_6",
  [KEY_NUM_7] = "KEY_NUM_7",
  [KEY_NUM_8] = "KEY_NUM_8",
  [KEY_NUM_9] = "KEY_NUM_9",
  [KEY_NUM_0] = "KEY_NUM_0",
  [KEY_NUM_DOT] = "KEY_NUM_DOT",
  [KEY_INTL_BACKSLASH] = "KEY_INTL_BACKSLASH",
  [KEY_MENU] = "KEY_MENU",
  [KEY_CTRL_LEFT] = "KEY_CTRL_LEFT",
  [KEY_SHIFT_LEFT] = "KEY_SHIFT_LEFT",
  [KEY_ALT_LEFT] = "KEY_ALT_LEFT",
  [KEY_WIN_LEFT] = "KEY_WIN_LEFT",
  [KEY_CTRL_RIGHT] = "KEY_CTRL_RIGHT",
  [KEY_SHIFT_RIGHT] = "KEY_SHIFT_RIGHT",
  [KEY_ALT_RIGHT] = "KEY_ALT_RIGHT",
  [KEY_WIN_RIGHT] = "KEY_WIN_RIGHT"
};

static size_t keyboard_key_name_length = sizeof(keyboard_key_name) / sizeof(char *);

const char *keyboard_key_get_name(keyboard_key_t key)
{
  // Key out of range
  if (key < 0 || key >= keyboard_key_name_length) return NULL;
  return keyboard_key_name[key];
}

keyboard_key_t keyboard_key_get_key(const char *name)
{
  // No name provided
  if (!name) return 0;

  // Search through all keys
  for (size_t i = 0; i < keyboard_key_name_length; i++)
  {
    // Compare and return if the name matched
    const char *curr = keyboard_key_name[i];
    if (strncasecmp(name, curr, strlen(curr)) == 0) return i;
  }

  // Key not found
  return 0;
}