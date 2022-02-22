keyrows = [
  ["KEY_ESC", "KEY_F1", "KEY_F2", "KEY_F3", "KEY_F4", "KEY_F5", "KEY_F6", "KEY_F7", "KEY_F8", "KEY_F9", "KEY_F10", "KEY_F11", "KEY_F12", "KEY_PRINT_SCREEN", "KEY_SCROLL_LOCK", "KEY_PAUSE_BREAK"],
  ["KEY_TILDE", "KEY_N1", "KEY_N2", "KEY_N3", "KEY_N4", "KEY_N5", "KEY_N6", "KEY_N7", "KEY_N8", "KEY_N9", "KEY_N0", "KEY_MINUS", "KEY_EQUAL", "KEY_BACKSPACE", "KEY_INSERT", "KEY_HOME", "KEY_PAGE_UP"],
  ["KEY_TAB", "KEY_Q", "KEY_W", "KEY_E", "KEY_R", "KEY_T", "KEY_Y", "KEY_U", "KEY_I", "KEY_O", "KEY_P", "KEY_OPEN_BRACKET", "KEY_CLOSE_BRACKET", "KEY_ENTER", "KEY_DEL", "KEY_END", "KEY_PAGE_DOWN"],
  ["KEY_CAPS_LOCK", "KEY_A", "KEY_S", "KEY_D", "KEY_F", "KEY_G", "KEY_H", "KEY_J", "KEY_K", "KEY_L", "KEY_SEMICOLON", "KEY_QUOTE", "KEY_DOLLAR"],
  ["KEY_SHIFT_LEFT", "KEY_INTL_BACKSLASH", "KEY_Z", "KEY_X", "KEY_C", "KEY_V", "KEY_B", "KEY_N", "KEY_M", "KEY_COMMA", "KEY_PERIOD", "KEY_SLASH", "KEY_SHIFT_RIGHT", "KEY_ARROW_TOP"],
  ["KEY_CTRL_LEFT", "KEY_WIN_LEFT", "KEY_ALT_LEFT", "KEY_SPACE", "KEY_ALT_RIGHT", "KEY_WIN_RIGHT", "KEY_MENU", "KEY_CTRL_RIGHT", "KEY_ARROW_LEFT", "KEY_ARROW_BOTTOM", "KEY_ARROW_RIGHT"]
]

print("""
[settings]
frame_del=90
draw_mode=KDM_ADD_ALL
; mapping_lang=de
""")

prev_key = None
frame_counter = 1
for row in keyrows:
  for col in row:
    print(f"[{frame_counter}]")
    if prev_key:
      print(f"{prev_key}=000000")
    print(f"{col}=FF0000")
    prev_key = col
    frame_counter += 1
    print()