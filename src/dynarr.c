#include "dynarr.h"

/**
 * @brief Clean up a no longer needed dynarr struct and all of it's items
 */
INLINED static void dynarr_cleanup(mman_meta_t *ref)
{
  dynarr_t *dynarr = (dynarr_t *) ref->ptr;

  // Clean up items if applicable
  if (dynarr->_cf)
  {
    for (size_t i = 0; i < dynarr->_array_size; i++)
      dynarr->_cf(dynarr->items[i]);
  }

  // Dealloc the item pointers
  mman_dealloc(dynarr->items);
}

dynarr_t *dynarr_make(size_t array_size, size_t array_max_size, cleanup_fn_t cf)
{
  scptr dynarr_t *res = mman_alloc(sizeof(dynarr_t), 1, dynarr_cleanup);

  res->_array_cap = array_max_size; // no freeing
  res->_array_size = array_size; // no freeing
  res->_cf = cf; // no freeing

  // Allocate all slots and initialize them to NULL
  res->items = (void *) mman_alloc(sizeof(void *), array_size, NULL); // needs mman freeing
  for (size_t i = 0; i < array_size; i++)
    res->items[i] = NULL;

  return mman_ref(res);
}

INLINED static void dynarr_resize_arr(dynarr_t *arr, size_t new_size)
{
  // Resize memory block of the array
  arr->items = mman_realloc((void **) &arr->items, sizeof(void *), new_size)->ptr;
  
  // Initialize new slots
  for (size_t i = arr->_array_size; i < new_size; i++)
    arr->items[i] = NULL;
  
  // Keep track of the new size
  arr->_array_size = new_size;
}

static bool dynarr_try_resize(dynarr_t *arr)
{
  size_t rem_cap = arr->_array_cap - arr->_array_size;
  if (rem_cap > 0)
  {
    // Try to double the amount of slots, go straight to the cap otherwise
    size_t new_size = arr->_array_size * 2;
    if (new_size > rem_cap)
      new_size = arr->_array_size + rem_cap;

    // Resized
    dynarr_resize_arr(arr, new_size);
    return true;
  }

  // Can't go any further
  return false;
}

dynarr_result_t dynarr_push(dynarr_t *arr, void *item, size_t *slot)
{
  // Iterate from tail to head
  for (size_t i = 0; i < arr->_array_size; i++)
  {
    // Search for a free slot
    if (arr->items[i] != NULL) continue;

    // Set item
    arr->items[i] = item;
    if (slot) *slot = i;
    return dynarr_SUCCESS;
  }

  if (dynarr_try_resize(arr))
    return dynarr_push(arr, item, slot);

  // No more free slots
  return dynarr_FULL;
}

dynarr_result_t dynarr_set_at(dynarr_t *arr, size_t index, void *item)
{
  // Index range check
  if (index < 0 || index >= arr->_array_size) return dynarr_INDEX_NOT_FOUND;

  // Free old entry, if any
  void *slot = arr->items[index];
  if (slot) arr->_cf(slot);

  slot = item;
  return dynarr_SUCCESS;
}

dynarr_result_t dynarr_pop(dynarr_t *arr, void **out, size_t *slot)
{
  // Iterate from tail to head
  for (size_t i = arr->_array_size - 1; i >= 0; i--)
  {
    // Search for an occupied slot
    if (arr->items[i] == NULL) continue;

    // Remove this item
    if (slot) *slot = i;
    return dynarr_remove_at(arr, i, out);
  }

  // No occupied slots
  return dynarr_EMPTY;
}

dynarr_result_t dynarr_remove_at(dynarr_t *arr, size_t index, void **out)
{
  // Range check
  if (index < 0 || index >= arr->_array_size) return dynarr_INDEX_NOT_FOUND;

  // Write pointer to output buffer, clear slot
  void *slot = arr->items[index];
  if (out) *out = slot;
  slot = NULL;
  return dynarr_SUCCESS;
}

char *dynarr_dump_hr_strs(dynarr_t *arr)
{
  return dynarr_dump_hr(arr, NULL);
}

char *dynarr_dump_hr(dynarr_t *arr, stringifier_t stringifier)
{
  // Allocate buffer for formatting strings into
  scptr char *buf = mman_alloc(sizeof(char), 128, NULL);
  size_t buf_offs = 0;

  // Array start marker
  if (!strfmt(&buf, &buf_offs, "[")) return NULL;

  for (size_t i = 0; i < arr->_array_size; i++)
  {
    // Skip empty slots
    void *item = arr->items[i];
    if (!item) continue;

    // Print slot string with quotes and comma-separators
    if (!strfmt(
      &buf, &buf_offs,
      "%s\"%s\"",
      i == 0 ? "" : ", ",
      stringifier ? stringifier(item) : (char *) item
    )) return NULL;
  }

  // Array end marker
  if (!strfmt(&buf, &buf_offs, "]")) return NULL;
  return mman_ref(buf);
}

size_t dynarr_as_array(dynarr_t *arr, void ***out)
{
  // No output buffer provided
  if (!out) return 0;

  // Count number of active slots
  size_t active_slots = 0;
  for (size_t i = 0; i < arr->_array_size; i++)
    if (arr->items[i]) active_slots++;

  if (active_slots == 0) return 0;

  // Create array
  scptr void **res = (void **) mman_alloc(sizeof(void *), active_slots, NULL);

  // Copy over pointers
  size_t res_index = 0;
  for (size_t i = 0; i < arr->_array_size; i++)
  {
    void *item = arr->items[i];
    if (item) res[res_index++] = item;
  }

  *out = mman_ref(res);
  return active_slots;
}