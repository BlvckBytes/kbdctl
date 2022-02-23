#ifndef keyboard_h
#define keyboard_h

#include <hidapi/hidapi.h>

#include "util/strfmt.h"
#include "util/strclone.h"
#include "util/strconv.h"
#include "util/compattrs.h"
#include "util/mman.h"
#include "util/dbglog.h"

/**
 * @brief Represents the keyboard device and all it's necessary data
 */
typedef struct keyboard
{
  char *serial_str;               // Product serial number
  char *manufacturer;             // Manufacturer name
  char *product;                  // Product name

  uint16_t vendor_id;             // USB Vendor ID
  uint16_t product_id;            // USB Product ID
  wchar_t *serial;                // Device serial
  char *path;                     // System path for this device

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
 * @brief Dump a keyboard's important fields as string
 */
char *keyboard_dump(keyboard_t *kb);

/**
 * @brief Open a connection to the keyboard
 * 
 * @param kb Keyboard to connect
 * @param err Error, leave NULL if not interested
 * 
 * @return true Successfully connected
 * @return false Error during connection trial, maybe forgot sudo?
 */
bool keyboard_open(keyboard_t *kb, char **err);

/**
 * @brief Close an existing keyboard connection, do nothing otherwise
 * 
 * @param kb Keyboard to connect
 */
void keyboard_close(keyboard_t *kb);

/**
 * @brief Transmit a sequence of bytes to the keyboard
 * 
 * @param kb Keyboard reference
 * @param data Sequence of bytes to send
 * @param data_len Number of bytes in that sequence
 * 
 * @return true Successfully sent
 * @return false Could not send, maybe forgot to open first?
 */
bool keyboard_transmit(keyboard_t *kb, uint8_t *data, size_t data_len);

#endif