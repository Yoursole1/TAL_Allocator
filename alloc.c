#include "stdint.h"
#include "stdbool.h"
#include <stddef.h>


struct Allocator{
    uint32_t heap_size;
    void* heap_start;

    /**
    * Tells you if the given block of memory is free
    */
    bool(*isFree)(void* self, void* mem);

    /**
     * Allocates a block of a given size, then returns a pointer to this block
     * 
     * Returns a NULL pointer if there is not enough space to allocate this block
     */
    void*(*alloc)(void* self, uint32_t size);

    /**
     * Free the block of a given pointer
     */
    void(*free)(void* self, void* mem);
};
