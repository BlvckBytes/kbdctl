# INFO: This script is used to quickly generate animations which the C code can then dispatch

from io import TextIOWrapper
import os
from typing import Callable, List, TextIO, Tuple
from animations import snake

# Create output folder if not yet existing
user_dir = os.path.expanduser('~')
out_dir = os.path.join(user_dir, ".config/kbdctl/animations")
if not os.path.exists(out_dir):
  os.makedirs(out_dir)

# Register animation generators and their descriptive name here
animations: List[Tuple[str, Callable[[TextIO], None]]] = [
  ("snake", snake.generate)
]

# Create all files
for fname, animfunc in animations:
  f_path = os.path.join(out_dir, f"{fname}.ini")
  print(f'CREATING: "{f_path}"')
  with open(f_path, 'w') as f:
    animfunc(f)