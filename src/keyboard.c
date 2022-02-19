#include <keyboard.h>

static void keyboard_cleanup(mman_meta_t *meta)
{
  keyboard_t *kb = (keyboard_t *) meta->ptr;

  // Dealloc string dupes
  mman_dealloc(kb->manufacturer);
  mman_dealloc(kb->product);
  mman_dealloc(kb->serial_no);
}

void keyboard_print(keyboard_t *kb)
{
  printf(
    "Keyboard {man=" QUOTSTR ", ser=" QUOTSTR ", prod=" QUOTSTR "}\n",
    kb->manufacturer, kb->serial_no, kb->product
  );
}

keyboard_t *keyboard_from_hdi(struct hid_device_info *hdi)
{
  // Create and set HDI ref
  keyboard_t *kb = (keyboard_t *) mman_alloc(sizeof(keyboard_t), 1, keyboard_cleanup);
  kb->hdi = hdi;

  // Duplicate wchar strings into standard C strings

  kb->manufacturer = mman_alloc(sizeof(char), 512, NULL);
  wcstombs(kb->manufacturer, hdi->manufacturer_string, 512);

  kb->serial_no = mman_alloc(sizeof(char), 512, NULL);
  wcstombs(kb->serial_no, hdi->serial_number, 512);

  kb->product = mman_alloc(sizeof(char), 512, NULL);
  wcstombs(kb->product, hdi->product_string, 512);

  return mman_ref(kb);
}