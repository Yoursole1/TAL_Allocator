#include "stdint.h"
#include "stdbool.h"
#include <stddef.h>

/**
 * Tells you if the given block of memory is free
 */
bool isFree(void* mem);

/**
 * Allocates a block of a given size, then returns a pointer to this block
 * 
 * Returns a NULL pointer if there is not enough space to allocate this block
 */
void* alloc(uint32_t size);

/**
 * Free the block of a given pointer
 */
void free(void* mem);