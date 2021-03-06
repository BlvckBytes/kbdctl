#include "keyboard_devman.h"

char *keyboard_devman_list()
{
  scptr char *res = mman_alloc(sizeof(char), 128, NULL);
  size_t res_offs = 0;

  struct hid_device_info *henum, *dev;
  henum = hid_enumerate(0x0, 0x0);

  // Loop all known local devices
  for (dev = henum; dev; dev = dev->next)
  {
    // Convert required strings from wchar to cstr
    scptr char *man_str = strconv(dev->manufacturer_string, 128);
    scptr char *prod_str = strconv(dev->product_string, 128);
    scptr char *ser_str = strconv(dev->serial_number, 128);

    // Never use root interfaces as this would block typing
    if (dev->interface_number == 0)
      continue;

    // Append current device to buffer
    strfmt(
      &res, &res_offs,
      "VID=%04X (" QUOTSTR "), PID=%04X (" QUOTSTR "), SER=" QUOTSTR ", PATH=" QUOTSTR "\n",
      dev->vendor_id,
      STRFMT_EMPTYMARK(man_str),
      dev->product_id,
      STRFMT_EMPTYMARK(prod_str),
      STRFMT_EMPTYMARK(ser_str),
      dev->path
    );
  }

  // Free enumeration and return formatted result
  hid_free_enumeration(henum);
  return mman_ref(res);
}

keyboard_t *keyboard_devman_find(uint16_t vid, uint16_t pid, char *ser)
{
  struct hid_device_info *henum = NULL, *dev = NULL;
  henum = hid_enumerate(0x0, 0x0);

  // Loop all known local devices
  scptr keyboard_t *kb = NULL;
  for (dev = henum; dev; dev = dev->next)
  {
    scptr char *ser_str = strconv(dev->serial_number, 128);

    // Skip devices that are not of interest
    if (
      dev->vendor_id == vid                                   // Matching vendor id
      && dev->product_id == pid                               // Matching product id
      && (!ser || strncmp(ser, ser_str, strlen(ser)) == 0)    // Matching serial or no serial provided
      && dev->interface_number != 0                           // Not interface 0 (root, used for keypresses)
    )
    {
      // Convert and store
      kb = keyboard_from_hdi(dev);
      break;
    }
  }

  hid_free_enumeration(henum);

  // Return NULL if not found or the created kb
  if (!kb) return NULL;
  return mman_ref(kb);
}