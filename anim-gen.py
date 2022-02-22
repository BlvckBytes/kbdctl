# INFO: This script is used to quickly generate animations which the C code can then dispatch

# All key names as they occurr in rows, scanned from left to right
keyrows = [
  ["KEY_ESC", "KEY_F1", "KEY_F2", "KEY_F3", "KEY_F4", "KEY_F5", "KEY_F6", "KEY_F7", "KEY_F8", "KEY_F9", "KEY_F10", "KEY_F11", "KEY_F12", "KEY_PRINT_SCREEN", "KEY_SCROLL_LOCK", "KEY_PAUSE_BREAK"],
  ["KEY_TILDE", "KEY_N1", "KEY_N2", "KEY_N3", "KEY_N4", "KEY_N5", "KEY_N6", "KEY_N7", "KEY_N8", "KEY_N9", "KEY_N0", "KEY_MINUS", "KEY_EQUAL", "KEY_BACKSPACE", "KEY_INSERT", "KEY_HOME", "KEY_PAGE_UP"],
  ["KEY_TAB", "KEY_Q", "KEY_W", "KEY_E", "KEY_R", "KEY_T", "KEY_Y", "KEY_U", "KEY_I", "KEY_O", "KEY_P", "KEY_OPEN_BRACKET", "KEY_CLOSE_BRACKET", "KEY_ENTER", "KEY_DEL", "KEY_END", "KEY_PAGE_DOWN"],
  ["KEY_CAPS_LOCK", "KEY_A", "KEY_S", "KEY_D", "KEY_F", "KEY_G", "KEY_H", "KEY_J", "KEY_K", "KEY_L", "KEY_SEMICOLON", "KEY_QUOTE", "KEY_DOLLAR"],
  ["KEY_SHIFT_LEFT", "KEY_INTL_BACKSLASH", "KEY_Z", "KEY_X", "KEY_C", "KEY_V", "KEY_B", "KEY_N", "KEY_M", "KEY_COMMA", "KEY_PERIOD", "KEY_SLASH", "KEY_SHIFT_RIGHT", "KEY_ARROW_TOP"],
  ["KEY_CTRL_LEFT", "KEY_WIN_LEFT", "KEY_ALT_LEFT", "KEY_SPACE", "KEY_ALT_RIGHT", "KEY_WIN_RIGHT", "KEY_MENU", "KEY_CTRL_RIGHT", "KEY_ARROW_LEFT", "KEY_ARROW_BOTTOM", "KEY_ARROW_RIGHT"]
]

# Snake body buffer and it's max length
snake_body = []
snake_color = "FF00FF"
snake_speed = 30
snake_size = 6

# Print the settings section
print(
f"""[settings]
frame_del={snake_speed}
draw_mode=KDM_ADD_ALL
; mapping_lang=de
""")

# Handle appending another key to the body while
# popping off the tail if it exceeds the length
def snake_append(key):
  snake_body.append(key)
  print(f"{key}={snake_color}")
  if (len(snake_body) > snake_size):
    dis_key = snake_body.pop(0)
    print(f"{dis_key}=000000")

# Frame counter for section headings
frame_counter = 1
def handle_frame(key):
  global frame_counter

  # Print the section head as well as the
  # snake body delta, followed by a newline for readability
  print(f"[{frame_counter}]")
  snake_append(key)
  print()

  frame_counter += 1

# Create serpentines
for rowi in range(0, len(keyrows)):
  row = keyrows[rowi]
  rowlen = len(row)
  for coli in range(0, rowlen):
    # Reverse uneven rows
    i = coli if rowi % 2 == 0 else rowlen - coli - 1
    handle_frame(row[i])

# Go back to the start
for rowi in reversed(range(0, len(keyrows) - 1)):
  handle_frame(keyrows[rowi][0])