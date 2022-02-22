#ifndef keyboard_group_addr_h
#define keyboard_group_addr_h

// The group's address a keyboard item is residing in
typedef enum keyboard_group_addr
{
  KGA_STATUS    = 0x4005,       // Status indicators
  KGA_KEY       = 0x010E        // Keys
} keyboard_group_addr_t;

#endif