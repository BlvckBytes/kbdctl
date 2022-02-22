#ifndef keyboard_devman_h
#define keyboard_devman_h

#include "hidapi/hidapi.h"

#include "util/mman.h"
#include "util/strfmt.h"
#include "util/strconv.h"

/**
 * @brief List all available USB devices in human readable format
 * @return char* List of devices
 */
char *keyboard_devman_list();

#endif