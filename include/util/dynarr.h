#ifndef dynarr_h
#define dynarr_h

#include <stddef.h>
#include <stdbool.h>

#include "util/mman.h"
#include "util/strfmt.h"
#include "util/common_types.h"

/**
 * @brief Represents the dynamic array, keeping track of it's
 * size and cleanup method
 */
typedef struct
{
  // Actual table
  void **items;

  // Current allocated size of the array
  size_t _array_size;

  // Maximum size the array can grow to
  size_t _array_cap;

  // Cleanup function for the array items
  cleanup_fn_t _cf;
} dynarr_t;

typedef enum
{
  // Successful operation
  dynarr_SUCCESS,

  // Key at requested index not existing
  dynarr_INDEX_NOT_FOUND,

  // No more space for more items
  dynarr_FULL,

  // No more item to pop
  dynarr_EMPTY,
} dynarr_result_t;

/**
 * @brief Make a new, empty array
 * 
 * @param array_size Size of the array
 * @param array_max_size Maximum size of the array, set to array_size for no automatic growth
 * @param cf Cleanup function for the items
 * @return dynarr_t* Pointer to the new array
 */
dynarr_t *dynarr_make(size_t array_size, size_t array_max_size, cleanup_fn_t cf);

/**
 * @brief Push a new item into the array
 * 
 * @param arr Array reference
 * @param item Item to push
 * @param slot Slot that has been pushed to, set to NULL if not needed
 * @return dynarr_result_t Operation result
 */
dynarr_result_t dynarr_push(dynarr_t *arr, void *item, size_t *slot);

/**
 * @brief Set an item at a specific location in the array
 * 
 * @param arr Array reference
 * @param index Array index
 * @param item Item to set
 * @return dynarr_result_t Operation result
 */
dynarr_result_t dynarr_set_at(dynarr_t *arr, size_t index, void *item);

/**
 * @brief Pop an existing item off the array
 * 
 * @param arr Array reference
 * @param out Output pointer buffer
 * @param slot Slot that has been popped off, set to NULL if not needed
 * @return dynarr_result_t Operation result
 */
dynarr_result_t dynarr_pop(dynarr_t *arr, void **out, size_t *slot);

/**
 * @brief Remove an item at a specific location from the array
 * 
 * @param arr Array reference
 * @param index Array index
 * @return dynarr_result_t Operation result
 */
dynarr_result_t dynarr_remove_at(dynarr_t *arr, size_t index, void **out);

/**
 * @brief Dumps the current state of the array in a human readable format
 * 
 * @param arr Array to dump
 * @param stringifier Stringifier function to apply for values, leave as NULL for internal casting
 * @return char* Formatted result string
 */
char *dynarr_dump_hr(dynarr_t *arr, stringifier_t stringifier);

/**
 * @brief Dumps the current state of the array containing string values
 * in a human readable format
 * 
 * @param arr Array to dump
 * @return char* Formatted result string
 */
char *dynarr_dump_hr_strs(dynarr_t *arr);

/**
 * @brief Get as a standard C array
 * 
 * @param arr Array to get
 * @param out Buffer of result
 * @return size_t Size of resulting array
 */
size_t dynarr_as_array(dynarr_t *arr, void ***out);

/**
 * @brief Clear the whole array by clearing all individual slots
 * 
 * @param arr Array to clear
 */
void dynarr_clear(dynarr_t *arr);

#endif