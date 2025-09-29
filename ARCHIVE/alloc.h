#pragma once
#include <stdint.h>
#include <stdbool.h>

struct alloc_result {
 void* mem;

 // 1 for pool allocation
 // 0 for general allocation
 // -1 for failed allocation
 int pool;
};
/**
 * Builds the two internal heaps and initializes the allocator
 */
void init_heap(void* pool_start, void* general_start);

/**
* Tells you if the given block of memory is free
*/
bool isFree(void* mem);

/**
* Allocates a block of a given size, then returns a pointer to this block
* 
* Returns a NULL pointer if there is not enough space to allocate this block
*/
struct alloc_result alloc(uint32_t size);

/**
* Free the block of a given pointer
*/
void free(void* mem);