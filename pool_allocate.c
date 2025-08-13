
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "alloc_strategy.h"

#include <stdio.h>





// SETTINGS ------------------------- 

#define HEAP_SIZE 2048 // Tile game be like 


#define MAX_POOL_SIZE 256 // size of largest pool
#define EXP 8 // 2^EXP = MAX_POOL_SIZE, defined for ease of use
#define IS_FREE_SIZE 1 + ((10 + 9 + 5 + 4 + 3) / 8)


// ----------------------------------
struct block_t{
    struct block_t* next_block;
};

struct Pool_Allocator{

    struct Alloc_Strategy super;

    uint8_t* is_free;
    struct block_t** pool_heads;

    uint32_t* pool_counts;
    uint32_t* pool_size;

    uint32_t (*get_index)(void* self, void* block);
    uint32_t (*get_pool)(void* self, void* block);
    struct block_t* (*build_pool)(void* self, void* curr, uint32_t pool_size, uint32_t pool_count, uint32_t i);

};



// get the index of a block in the is_free list
uint32_t get_index(void* self, void* block){
    struct Pool_Allocator this = *(struct Pool_Allocator*)(self);
    /**
     * This function really annoyed me.  Basically we need the amount of total blocks before this pointer
     * (which is its index ofc), but I can not think of a way to do this in constant time.  Maybe some sort
     * of hash function would do.  For now I am writing this in linear time to just work for testing, 
     * and maybe sometime in the future we can optimize it (I can think of a way to get it to logorithmic time). 
     * Reguardless, I feel like it should be possible in constant time so if you're reading this and are
     * bored then I would be forever grateful if you could make this fast.
     */

    if((uint8_t*)block < (uint8_t*)this.super.heap_start || (uint8_t*)block > ((uint8_t*)this.super.heap_start) + HEAP_SIZE){ // out of range
        return -1;
    }

    uint8_t* blk = (uint8_t*) block;

    uint32_t i = 0;
    uint32_t pool_left = this.pool_counts[i];

    uint32_t index = 0;

    while(blk > (uint8_t*)this.super.heap_start){
        if(pool_left <= 0){
            i++;
            pool_left = this.pool_counts[i];
        }
        blk -= this.pool_size[i];
        pool_left -= 1;
        index++;
    }

    return index;
}

uint32_t get_pool(void* self, void* block){
    struct Pool_Allocator this = *(struct Pool_Allocator*)(self);

    if((uint8_t*)block < (uint8_t*)this.super.heap_start || (uint8_t*)block > ((uint8_t*)this.super.heap_start) + HEAP_SIZE){ // out of range
        return -1;
    }

    uint8_t* blk = (uint8_t*) block;

    uint32_t i = 0;
    uint32_t pool_left = this.pool_counts[i];

    while(blk >= (uint8_t*)this.super.heap_start){
        if(pool_left <= 0){
            i++;
            pool_left = this.pool_counts[i];
        }
        blk -= this.pool_size[i];
        pool_left -= 1;
    }

    return i;
}

struct block_t* build_pool(void* self, void* curr, uint32_t pool_size, uint32_t pool_count, uint32_t i)
{      
    struct Pool_Allocator this = *(struct Pool_Allocator*)(self);
    struct block_t* s = (struct block_t*)curr;

    if(i >= pool_count - 1){
        struct block_t n = {NULL};
        *s = n;
        return s;
    }

    struct block_t t = (struct block_t)
    {
        this.build_pool(self, (uint8_t*)curr + pool_size, pool_size, pool_count, i + 1)
    };
    *s = t;

    return s;
}



bool pool_isFree(void* self, void* mem)
{
    struct Pool_Allocator this = *(struct Pool_Allocator*)(self);

    uint32_t index = this.get_index(self, mem);
    if(index == -1){
        return false;
    }
    uint32_t big_index = index / 8;
    uint32_t small_index = index % 8;

    return (this.is_free[big_index] & (1 << small_index)) != 0;
}

void* pool_alloc(void* self, uint32_t size)
{   
    struct Pool_Allocator this = *(struct Pool_Allocator*)(self);
    // find ideal i
    uint32_t i = 0;
    for(; size > this.pool_size[i] && i < EXP - 3; i++);
    
    // if the pool for ideal i is already full (null head), keep going to next block until we find a free one
    struct block_t* block = this.pool_heads[i];
    while(block == NULL && i < EXP - 3){
        i++;
        block = this.pool_heads[i];
    }

    if(block == NULL){
        return NULL; // no space for a new block of this size
    }

    // update free blocks
    uint32_t index = get_index(self, block);
    uint32_t big_index = index / 8;
    uint32_t small_index = index % 8;

    // update linked list
    this.pool_heads[i] = block->next_block;

    this.is_free[big_index] &= ~((uint8_t)1 << small_index);

    for(int i = 0; i < this.pool_size[i]; i++){ // zero this block before returning
        *(((uint8_t*)block) + i) = 0;
    }

    return block; 
}


void pool_free(void* self, void* mem)
{
    struct Pool_Allocator this = *(struct Pool_Allocator*)(self);

    uint32_t i = get_pool(self, mem);
    if(i == -1){
        return; // bad call, already "free" since its not in heap
    }

    struct block_t new_head = (struct block_t){
        this.pool_heads[i]
    };

    *((struct block_t*)mem) = new_head;

    this.pool_heads[i] = (struct block_t*)mem;

    uint32_t index = get_index(self, mem);
    uint32_t big_index = index / 8;
    uint32_t small_index = index % 8;

    this.is_free[big_index] |= (uint8_t)1 << small_index;
}











// pool_sizes[0] -> is for pool size of 8 bytes (64 bits, aka min pointer size on 64 bit arch)
static const uint32_t pool_counts[EXP - 3]  = {10, 9, 5, 4, 3}; // 1, 2, 4, 8 byte blocks do not exist, since we are going to store pointers in these (8 would be fine)
static const uint32_t pool_size[EXP - 3]    = {16, 32, 64, 128, 256};
/**
 * 16 + 16 + 10 + 9 + 6 is the total number blocks, so total number of bits we need to mark if they are free
 * (16 + 16 + 10 + 9 + 6) / 8 tells us the number of bytes for these needed (rounded down to nearest int)
 * 1 + ((16 + 16 + 10 + 9 + 6) / 8) gives us one extra byte, since if this is a decimal then we need an extra
 * NOTE: if (16 + 16 + 10 + 9 + 6) / 8 is not a decimal, then you don't need the + 1
 */

uint8_t is_free[IS_FREE_SIZE]; // if this param can be generated somehow I would loveee that 

struct block_t* pool_heads[EXP - 3];




struct Pool_Allocator build_pool_allocator(void* heap_start)
{
    struct Alloc_Strategy super = {
        .heap_size = HEAP_SIZE,
        .heap_start = heap_start,

        .isFree = &pool_isFree,
        .alloc = &pool_alloc,
        .free = &pool_free
    };



    uint32_t s = 0; 
    for(int i = 0; i < EXP - 3; i++){
        s += pool_size[i] * pool_counts[i];
    }

    if(s != HEAP_SIZE){
        return (struct Pool_Allocator){}; // configured pool distribution does not add up to the total heap size, it must
    }

    struct Pool_Allocator allocator = (struct Pool_Allocator){
        .super = super,
        .pool_heads = pool_heads,
        .is_free = is_free,
        
        .pool_counts = pool_counts,
        .pool_size = pool_size,

        .get_pool = &get_pool,
        .get_index = &get_index,
        .build_pool = &build_pool
    };

    void* start = heap_start;
    
    for(int i = 0; i < EXP - 3; i++){
        // build linked list for each pool
        pool_heads[i] = allocator.build_pool(&allocator, start, pool_size[i], pool_counts[i], 0);

        start += pool_size[i] * pool_counts[i]; // block size (bytes) * amount of blocks
    }

    

    for(int i = 0; i < IS_FREE_SIZE; i++){
        is_free[i] = 255;
    }

    allocator.pool_heads = &pool_heads;
    allocator.is_free = &is_free;

    return allocator;

}