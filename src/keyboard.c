#include <keyboard.h>

static void keyboard_cleanup(mman_meta_t *meta)
{
  keyboard_t *kb = (keyboard_t *) meta->ptr;

  // Dealloc string dupes
  mman_dealloc(kb->manufacturer);
  mman_dealloc(kb->product);
  mman_dealloc(kb->serial_str);
  mman_dealloc(kb->serial);
}

void keyboard_print(keyboard_t *kb)
{
  printf(
    "Keyboard {man=" QUOTSTR ", ser=" QUOTSTR ", prod=" QUOTSTR "}\n",
    kb->manufacturer, kb->serial_str, kb->product
  );
}

keyboard_t *keyboard_from_hdi(struct hid_device_info *hdi)
{
  // Create and set default values
  keyboard_t *kb = (keyboard_t *) mman_alloc(sizeof(keyboard_t), 1, keyboard_cleanup);

  // Copy serial number if hdi has a value
  if (hdi->serial_number)
  {
    kb->serial = (wchar_t *) mman_alloc(sizeof(wchar_t), wcslen(hdi->serial_number), NULL);
    wcscpy(kb->serial, hdi->serial_number);
  }
  else kb->serial = NULL;

  // kb->serial = hdi->serial_number;
  kb->vendor_id = hdi->vendor_id;
  kb->product_id = hdi->product_id;
  kb->handle = NULL;

  // Duplicate wchar strings into standard C strings
  kb->manufacturer = strconv(hdi->manufacturer_string, 512);
  kb->serial_str = strconv(hdi->serial_number, 512);
  kb->product = strconv(hdi->product_string, 512);

  return mman_ref(kb);
}

bool keyboard_open(keyboard_t *kb)
{
  if (kb->handle)
  {
    fprintf(stderr, "This keyboard is already connected!\n");
    return false;
  }

  kb->handle = hid_open(
    kb->vendor_id,
    kb->product_id,
    kb->serial
  );

  if (!kb->handle)
  {
    scptr char *err = strconv(hid_error(NULL), 1024);
    fprintf(stderr, "Could not open device connection! (%s)\n", err);
    return false;
  }

  return true;
}

void keyboard_close(keyboard_t *kb)
{
  if (!kb->handle) return;
  hid_close(kb->handle);
  kb->handle = NULL;
}