#include "mman.h"

static volatile size_t mman_alloc_count, mman_dealloc_count;

/*
============================================================================
                                 Meta Info                                  
============================================================================
*/

mman_meta_t *mman_fetch_meta(void *ptr)
{
  // Do nothing for nullptrs
  if (ptr == NULL) return NULL;

  // Fetch the meta info allocated before the data block and
  // assure that it's actually a managed resource
  mman_meta_t *meta = ptr - sizeof(mman_meta_t);
  if (ptr != meta->ptr)
  {
    fprintf(stderr, "Invalid resource passed to \"mman_fetch_meta\"!\n");
    return NULL;
  }

  return meta;
}

/*
============================================================================
                                 Allocation                                 
============================================================================
*/

/**
 * @brief Allocate a new meta-info structure as well as it's trailing data block
 * 
 * @param block_size Size of one data block in bytes
 * @param num_blocks Number of blocks with block_size
 * @param cf Cleanup function
 * @return mman_meta_t Pointer to the meta-info
 */
INLINED static mman_meta_t *mman_create(size_t block_size, size_t num_blocks, mman_cleanup_f_t cf)
{
  mman_meta_t *meta = (mman_meta_t *) malloc(
    sizeof(mman_meta_t) // Meta information
    + (block_size * num_blocks) // Data blocks
  );

  *meta = (mman_meta_t) {
    .ptr = meta + 1,
    .block_size = block_size,
    .num_blocks = num_blocks,
    .cf = cf,
    .refs = 1
  };

  return meta;
}

void *mman_alloc(size_t block_size, size_t num_blocks, mman_cleanup_f_t cf)
{
  // INFO: Increment the allocation count for debugging purposes
  atomic_increment(&mman_alloc_count);

  // Create new meta-info and return a pointer to the data block
  return mman_create(block_size, num_blocks, cf)->ptr;
}

mman_meta_t *mman_realloc(void **ptr_ptr, size_t block_size, size_t num_blocks)
{
  // Receiving a pointer to the pointer to the reference, deref once
  void *ptr = *ptr_ptr;

  // Fetch the meta info allocated before the data block
  mman_meta_t *meta = ptr - sizeof(mman_meta_t);
  if (ptr != meta->ptr)
  {
    fprintf(stderr, "Invalid resource passed to \"mman_realloc\"!\n");
    return NULL;
  }

  // Reallocate whole meta object
  meta = realloc(meta,
    sizeof(mman_meta_t) // Meta information
    + (block_size * num_blocks) // Data blocks
  );

  // Update the copied meta-block
  meta->ptr = meta + 1;
  meta->block_size = block_size;
  meta->num_blocks = num_blocks;

  // Update the outside pointer
  *ptr_ptr = meta->ptr;
  return meta;
}

/*
============================================================================
                                Deallocation                                
============================================================================
*/

INLINED static bool mman_dealloc_direct(mman_meta_t *meta)
{
  // Do nothing for nullptrs
  if (!meta) return false;

  // Call additional cleanup function, if applicable
  if (meta->cf) meta->cf(meta);

  // Free the whole allocated (meta- + data-) blocks by the head-ptr
  free(meta);
  meta = NULL;

  // Successful deallocation
  return true;
}

void mman_dealloc(void *ptr)
{
  mman_meta_t *meta = mman_fetch_meta(ptr);
  if (!meta) return;

  if (mman_dealloc_direct(meta))
    // INFO: Increment the deallocation count for debugging purposes
    atomic_increment(&mman_dealloc_count);
}

void mman_attr_dealloc(void *ptr_ptr)
{
  mman_meta_t *meta = mman_fetch_meta(*((void **) ptr_ptr));
  if (!meta) return;

  // Decrease number of active references
  // Do nothing as long as active references remain
  if (atomic_decrement(&meta->refs) > 0) return;

  mman_dealloc_direct(meta);
}

/*
============================================================================
                                 Referencing                                
============================================================================
*/

void *mman_ref(void *ptr)
{
  mman_meta_t *meta = mman_fetch_meta(ptr);
  if (!meta) return NULL;

  // Increment number of references and return pointer to the data block
  atomic_increment(&meta->refs);
  return meta->ptr;
}

/*
============================================================================
                                  Debugging                                 
============================================================================
*/

void mman_print_info()
{
  printf("----------< MMAN Statistics >----------\n");
  printf("> Allocated: %lu\n", mman_alloc_count);
  printf("> Deallocated: %lu\n", mman_alloc_count);
  printf("----------< MMAN Statistics >----------\n");
}