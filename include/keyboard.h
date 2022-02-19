#ifndef keyboard_h
#define keyboard_h

#include <hidapi/hidapi.h>

#include "mman.h"

typedef struct keyboard
{
  char *serial_no;                // Product serial number
  char *manufacturer;             // Manufacturer name
  char *product;                  // Product name

  struct hid_device_info *hdi;    // hidapi handle
} keyboard_t;

/**
 * @brief Create a new keyboard from it's corresponding device info
 * 
 * @param hdi Device info to extract data from and to link
 * @return keyboard_t* Allocated keyboard, MMAN managed
 */
keyboard_t *keyboard_from_hdi(struct hid_device_info *hdi);

/**
 * @brief Print a keyboard's important fields to stdout
 */
void keyboard_print(keyboard_t *kb);

#endif