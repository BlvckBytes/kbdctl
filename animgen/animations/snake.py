from typing import TextIO
from keyrows import keyrows

# Snake body buffer and it's max length
snake_body = []
snake_color = "FF00FF"
snake_speed = 30
snake_size = 6

# Handle appending another key to the body while
# popping off the tail if it exceeds the length
def snake_append(f: TextIO, key: str):
  snake_body.append(key)
  f.write(f"{key}={snake_color}\n")
  if (len(snake_body) > snake_size):
    dis_key = snake_body.pop(0)
    f.write(f"{dis_key}=000000\n")

# Frame counter for section headings
frame_counter = 1
def handle_frame(f: TextIO, key: str):
  global frame_counter

  # Print the section head as well as the
  # snake body delta, followed by a newline for readability
  f.write(f"[{frame_counter}]\n")
  snake_append(f, key)
  f.write("\n")

  frame_counter += 1

def generate(f: TextIO):
  # Print the settings section
  f.write("[settings]\n")
  f.write("frame_del={snake_speed}\n")
  f.write("draw_mode=KDM_ADD_ALL\n\n")

  # Create serpentines
  for rowi in range(0, len(keyrows)):
    row = keyrows[rowi]
    rowlen = len(row)
    for coli in range(0, rowlen):
      # Reverse uneven rows
      i = coli if rowi % 2 == 0 else rowlen - coli - 1
      handle_frame(f, row[i])

  # Go back to the start
  for rowi in reversed(range(0, len(keyrows) - 1)):
    handle_frame(f, keyrows[rowi][0])