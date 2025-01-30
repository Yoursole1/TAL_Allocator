

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "alloc.h"


// SETTINGS ------------------------- 

#define HEAP_SIZE 1024
// CHANGE, currently being set up in main, use macro
void* heap_start = (void*)0; 

// ----------------------------------

#define MAX_POOL_SIZE 256 // actually ends up being 2^n - 1 for all these sizes because first byte 
#define EXP 8 // 2^EXP = MAX_POOL_SIZE, defined for ease of use

uint32_t pool_sizes[EXP];



void init_heap()
{
    // pooled space = HEAPSIZE / 2

    for(int i = pool_sizes - 1; i >= 0; i--){
        pool_sizes[i] = uhhhhh idk;
    }
}

bool isFree(void* mem)
{
    return false;
}

void* alloc(uint32_t size)
{   
    // if not init, init_heap()

    return NULL; // no block could be found, possible fragmentation issue ):
}


void free(void* mem)
{
    
}