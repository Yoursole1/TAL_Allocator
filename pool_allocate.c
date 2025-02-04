

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "alloc.h"

#include <stdio.h>




// SETTINGS ------------------------- 

#define HEAP_SIZE 2048 // Tile game be like 

// CHANGE, currently being set up in main, use macro
void* heap_start = (void*)0; 


// ----------------------------------

struct Pool_Allocator{

    struct Allocator super;

};



#define MAX_POOL_SIZE 256 // size of largest pool
#define EXP 8 // 2^EXP = MAX_POOL_SIZE, defined for ease of use

// pool_sizes[0] -> is for pool size of 8 bytes (64 bits, aka min pointer size on 64 bit arch)
static const uint32_t pool_counts[EXP - 3]  = {16, 16, 10, 9, 6}; // 1, 2, 4 byte blocks must not exist, since we are going to store pointers in these
static const uint32_t pool_size[EXP - 3]    = {16, 32, 64, 128, 256};
/**
 * 16 + 16 + 10 + 9 + 6 is the total number blocks, so total number of bits we need to mark if they are free
 * (16 + 16 + 10 + 9 + 6) / 8 tells us the number of bytes for these needed (rounded down to nearest int)
 * 1 + ((16 + 16 + 10 + 9 + 6) / 8) gives us one extra byte, since if this is a decimal then we need an extra
 * NOTE: if (16 + 16 + 10 + 9 + 6) / 8 is not a decimal, then you don't need the + 1
 */
uint8_t is_free[1 + ((16 + 16 + 10 + 9 + 6) / 8)]; // if this param can be generated somehow I would loveee that 

typedef struct block_t;
typedef struct {
    struct block_t* next_block;
} block_t;

_Static_assert(sizeof(block_t) <= 64);

block_t* pool_heads[EXP - 3];

void* init_heap()
{
    // pooled space = HEAPSIZE / 2
    int s = 0; 
    for(int i = 0; i < EXP; i++){
        s += pool_size[i] * pool_counts[i];
    }

    printf("%i", s);

    if(s != HEAP_SIZE){
        return NULL; // configured pool distribution does not add up to the total heap size, it must
    }

    for(int i = 0; i < EXP - 3; i++){
        // build linked list for each pool
        

    }
    
}

bool isFree(void* self, void* mem)
{
    return false;
}

void* alloc(void* self, uint32_t size)
{   
    // if not init, init_heap()

    return NULL; // no block could be found, possible fragmentation issue ):
}


void free(void* self, void* mem)
{
    
}