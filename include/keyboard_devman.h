#ifndef keyboard_devman_h
#define keyboard_devman_h

#include <hidapi/hidapi.h>
#include <string.h>

#include "util/mman.h"
#include "util/strfmt.h"
#include "util/strconv.h"
#include "keyboard.h"

/**
 * @brief List all available USB devices in human readable format
 * @return char* List of devices
 */
char *keyboard_devman_list();

/**
 * @brief Find a keyboard by it's vendor- and product IDs as well as
 * an optional serial number
 * 
 * @param vid Vendor ID of the device
 * @param pid Product ID of the device
 * @param ser Serial number, leave at NULL to take the first match of VID & PID
 * @return keyboard_t* Found keyboard or NULL if no matches occurred
 */
keyboard_t *keyboard_devman_find(uint16_t vid, uint16_t pid, char *ser);

#endif