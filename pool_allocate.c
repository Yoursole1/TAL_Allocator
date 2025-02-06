

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
static const uint32_t pool_counts[EXP - 3]  = {10, 9, 5, 4, 3}; // 1, 2, 4, 8 byte blocks do not exist, since we are going to store pointers in these (8 would be fine)
static const uint32_t pool_size[EXP - 3]    = {16, 32, 64, 128, 256};
/**
 * 16 + 16 + 10 + 9 + 6 is the total number blocks, so total number of bits we need to mark if they are free
 * (16 + 16 + 10 + 9 + 6) / 8 tells us the number of bytes for these needed (rounded down to nearest int)
 * 1 + ((16 + 16 + 10 + 9 + 6) / 8) gives us one extra byte, since if this is a decimal then we need an extra
 * NOTE: if (16 + 16 + 10 + 9 + 6) / 8 is not a decimal, then you don't need the + 1
 */
#define IS_FREE_SIZE 1 + ((10 + 9 + 5 + 4 + 3) / 8)
uint8_t is_free[IS_FREE_SIZE]; // if this param can be generated somehow I would loveee that 


typedef struct {
    struct block_t* next_block;
} block_t;

block_t* pool_heads[EXP - 3];

// ---------
// TODO ADD SAFETY TO BOTH get_index AND get_pool SO THAT IF A BAD POINTER IS PASSED TO THEM THEY DON'T LOOP FOREVER
// ---------

// get the index of a block in the is_free list
uint32_t get_index(void* block){
    /**
     * This function really annoyed me.  Basically we need the amount of total blocks before this pointer
     * (which is its index ofc), but I can not think of a way to do this in constant time.  Maybe some sort
     * of hash function would do.  For now I am writing this in linear time to just work for testing, 
     * and maybe sometime in the future we can optimize it (I can think of a way to get it to logorithmic time). 
     * Reguardless, I feel like it should be possible in constant time so if you're reading this and are
     * bored then I would be forever grateful if you could make this fast.
     */

    uint8_t* blk = (uint8_t*) block;

    uint32_t i = 0;
    uint32_t pool_left = pool_counts[i];

    uint32_t index = 0;

    while(blk > heap_start){
        if(pool_left <= 0){
            i++;
            pool_left = pool_counts[i];
        }
        blk -= pool_size[i];
        pool_left -= 1;
        index++;
    }

    return index;
}

uint32_t get_pool(void* block){
    uint8_t* blk = (uint8_t*) block;

    uint32_t i = 0;
    uint32_t pool_left = pool_counts[i];

    while(blk >= heap_start){
        if(pool_left <= 0){
            i++;
            pool_left = pool_counts[i];
        }
        blk -= pool_size[i];
        pool_left -= 1;
    }

    return i;
}

block_t* build_pool(void* curr, uint32_t pool_size, uint32_t pool_count, uint32_t i)
{      
    block_t* s = (block_t*)curr;

    if(i >= pool_count - 1){
        block_t n = {NULL};
        *s = n;
        return s;
    }

    block_t t = (block_t)
    {
        build_pool((uint8_t*)curr + pool_size, pool_size, pool_count, i + 1)
    };
    *s = t;

    return s;
}

void* init_heap()
{
    // pooled space = HEAPSIZE / 2
    uint32_t s = 0; 
    for(int i = 0; i < EXP - 3; i++){
        s += pool_size[i] * pool_counts[i];
    }

    if(s != HEAP_SIZE){
        return NULL; // configured pool distribution does not add up to the total heap size, it must
    }

    void* start = heap_start;
    
    for(int i = 0; i < EXP - 3; i++){
        // build linked list for each pool
        pool_heads[i] = build_pool(start, pool_size[i], pool_counts[i], 0);

        start += pool_size[i] * pool_counts[i]; // block size (bytes) * amount of blocks
    }

    for(int i = 0; i < IS_FREE_SIZE; i++){
        is_free[i] = 255;
    }
}



bool isFree(void* self, void* mem)
{
    uint32_t index = get_index(mem);
    uint32_t big_index = index / 8;
    uint32_t small_index = index % 8;

    return (is_free[big_index] & (1 << small_index)) != 0;
}

void* alloc(void* self, uint32_t size)
{   
    // find ideal i
    uint32_t i = 0;
    for(; size > pool_size[i] && i < EXP - 3; i++);
    
    // if the pool for ideal i is already full (null head), keep going to next block until we find a free one
    block_t* block = pool_heads[i];
    while(block == NULL && i < EXP - 3){
        i++;
        block = pool_heads[i];
    }

    if(block == NULL){
        return NULL; // no space for a new block of this size
    }

    // update free blocks
    uint32_t index = get_index(block);
    uint32_t big_index = index / 8;
    uint32_t small_index = index % 8;

    // update linked list
    pool_heads[i] = block->next_block;

    is_free[big_index] &= ~((uint8_t)1 << small_index);

    return block; // no block could be found, possible fragmentation issue ):
}


void free(void* self, void* mem)
{
    uint32_t i = get_pool(mem);

    block_t new_head = (block_t){
        pool_heads[i]
    };

    *((block_t*)mem) = new_head;

    pool_heads[i] = (block_t*)mem;

    uint32_t index = get_index(mem);
    uint32_t big_index = index / 8;
    uint32_t small_index = index % 8;

    is_free[big_index] |= (uint8_t)1 << small_index;
}