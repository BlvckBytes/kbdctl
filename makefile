CC        := gcc
CFLAGS    := -Wall
SRC_FILES := $(wildcard *.c) $(wildcard */*.c)

CPPFLAGS  := -I./include			# Local header files
CPPFLAGS  += -I/opt/homebrew/include		# Homebrew header files
CPPFLAGS  += -L/opt/homebrew/lib		# Homebrew libraries
CPPFLAGS  += -lhidapi				# Link against hidapi

OUT_FILE  := kbdctl

$(OUT_FILE):
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SRC_FILES) -o $(OUT_FILE)

clean:
	rm -rf $(OUT_FILE)
