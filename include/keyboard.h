#ifndef keyboard_h
#define keyboard_h

#include <hidapi/hidapi.h>

#include "strconv.h"
#include "mman.h"

typedef struct keyboard
{
  char *serial_str;               // Product serial number
  char *manufacturer;             // Manufacturer name
  char *product;                  // Product name

  uint16_t vendor_id;             // USB Vendor ID
  uint16_t product_id;            // USB Product ID
  wchar_t *serial;          // Device serial

  hid_device *handle;             // Device connection handle
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

/**
 * @brief Open a connection to the keyboard
 * 
 * @param kb Keyboard to connect
 * 
 * @return true Successfully connected
 * @return false Error during connection trial
 */
bool keyboard_open(keyboard_t *kb);

/**
 * @brief Close an existing keyboard connection, do nothing otherwise
 * 
 * @param kb Keyboard to connect
 */
void keyboard_close(keyboard_t *kb);

#endif