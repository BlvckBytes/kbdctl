#ifndef mman_h
#define mman_h

#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "common_macros.h"
#include "atomanip.h"

/*
============================================================================
                                   Macros                                   
============================================================================
*/

// Marks a memory managed variable
#define scptr __attribute__((cleanup(mman_attr_dealloc)))

/*
============================================================================
                                  Typedefs                                  
============================================================================
*/

// Forward ref
typedef struct mman_meta mman_meta_t;

/**
 * @brief Cleanup function used for external destructuring of a resource
 */
typedef void (*mman_cleanup_f_t)(mman_meta_t *);

/**
 * @brief Meta-information of a memory managed resource
 */
typedef struct mman_meta
{
  // Pointer to the resource
  void *ptr;

  // Size of one data block
  size_t block_size;

  // Number of blocks with block_size
  size_t num_blocks;

  // Cleanup function invoked before the resource gets free'd
  mman_cleanup_f_t cf;

  // Number of active references pointing at this resource
  volatile size_t refs;
} mman_meta_t;

/*
============================================================================
                                 Meta Info                                  
============================================================================
*/

/**
 * @brief Fetch the meta-block of a managed resource
 * 
 * @param ptr Pointer to the data-block of a managed resource
 * @return mman_meta_t* Retrieved meta-block or NULL if the resource is invalid
 */
mman_meta_t *mman_fetch_meta(void *ptr);

/*
============================================================================
                                 Allocation                                 
============================================================================
*/

/**
 * @brief Allocate memory and get a managed reference to it
 * 
 * @param block_size Size of one data block
 * @param size Number of blocks to allocate
 * @param cf Function for additional cleanup operations on pointers inside the data-block,
 * leave this as NULL when none exist and nothing has been allocated separately
 * @return void* Pointer to the resource, NULL if no space left
 */
void *mman_alloc(size_t block_size, size_t num_blocks, mman_cleanup_f_t cf);

/**
 * @brief Reallocate a managed datablock
 * 
 * @param ptr_ptr Pointer to the pointer to the resource
 * @param new_size New size of the data block
 * @return mman_meta_t* Pointer to the leading meta-block of the new data-block
 */
mman_meta_t *mman_realloc(void **ptr_ptr, size_t block_size, size_t num_blocks);

/*
============================================================================
                                Deallocation                                
============================================================================
*/

/**
 * @brief Deallocates a mman allocated resource manually
 * 
 * WARNING: This ignores the number of references and can be treated
 * as a force deallocator!
 * 
 * @param ptr Pointer to the resource
 */
void mman_dealloc(void *ptr);

/**
 * @brief Deallocate a managed resource when it goes out of scope and
 * has no references left pointing at it.
 * 
 * WARNING: Only to be called by GCC attributes, not manually!
 * 
 * @param ptr_ptr Pointer to the pointer to the resource
 */
void mman_attr_dealloc(void *ptr_ptr);

/*
============================================================================
                                 Referencing                                
============================================================================
*/

/**
 * @brief Create a new reference to be shared with other consumers
 * 
 * @param ptr Pointer to the managed resource
 * @return void* Pointer to be shared
 */
void *mman_ref(void *ptr);

/*
============================================================================
                                  Debugging                                 
============================================================================
*/

/**
 * @brief Prints informations about the current alloc/dealloc status on stdout
 */
void mman_print_info();

#endif