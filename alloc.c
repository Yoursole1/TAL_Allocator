//
// Created by Joshua Beard on 9/27/25.
//

#include "stdbool.h"
#include "stdint.h"

//----------------------------------------------------------------------------------
// BEGIN CONFIGURATION SECTION
// IF YOU CHANGE ANY OF THESE VALUES, IT IS HIGHLY LIKELY OTHERS WILL NEED TO CHANGE IN RESPONSE
//----------------------------------------------------------------------------------
// POINTER TO START OF HEAP
void* HEAP_START = (void*) 0x0;

#define NUMBER_OF_POOLS 4
// Must have POOL_BLOCK_SIZES[n + 1] > POOL_BLOCK_SIZES[n] && POOL_BLOCK_SIZES[0] >= sizeof(void*)
// The first rule is because of how the algorithm works
// The second is because all blocks need to be large enough to store a pointer (8 bytes for my system)
uint32_t POOL_BLOCK_SIZES[NUMBER_OF_POOLS] = {16, 32, 64, 128}; // note these do not need to be powers of two
uint32_t POOL_SIZES[NUMBER_OF_POOLS] = {2, 2, 3, 3};

// POOL_BLOCK_SIZES ⋅ POOL_SIZES, it would be nice to generate this line
// (16 * 2) + (32 * 2) + (64 * 3) + (128 * 3) = 672
#define TOTAL_HEAP_SIZE 672


// STEPS TO FIND IS_FREE_SIZE
// 1: SUM POOL SIZES (number of blocks total between all pools)
// 2: DIVIDE BY 8 (since is_free is an array of uint8_t's and each bit in the uint_8 represents if a
// block is free).
// 3.a: IF THIS IS AN INTEGER, THIS IS THE SIZE
// 3.b: IF THIS IS NOT AN INTEGER, ROUND UP

// Example:
// ceil [ sum(POOL_SIZES) / 8 ] => ceil[ (2 + 2 + 3 + 3) / 8 ] => 2
#define IS_FREE_SIZE 2

//----------------------------------------------------------------------------------
// END CONFIGURATION SECTION
//----------------------------------------------------------------------------------

struct block_t{
    struct block_t* next_block;
};


static uint8_t is_free[IS_FREE_SIZE];
static struct block_t* pool_heads[NUMBER_OF_POOLS];


/**
 * Internal function.  Builds the linked-list associated with each pool
 * @param curr initial head of the pool
 * @param pool_size size of blocks in this pool
 * @param pool_count number of blocks in this pool
 * @param i zero :)
 * @return
 */
struct block_t* build_pool(void* curr, uint32_t pool_size, uint32_t pool_count, uint32_t i)
{
    struct block_t* s = (struct block_t*)curr;

    if(i >= pool_count - 1){
        struct block_t n = {(void*)0}; // null terminator
        *s = n;
        return s;
    }

    struct block_t t = (struct block_t)
    {
        build_pool((uint8_t*)curr + pool_size, pool_size, pool_count, i + 1)
    };
    *s = t;

    return s;
}


/**
 * Internal function.
 *
 * Gets the number of blocks before the given block (its index in is_free)
 */
uint32_t get_index(void* block){
    /**
     * This function really annoyed me.  Basically we need the amount of total blocks before this pointer
     * (which is its index ofc), but I can not think of a way to do this in constant time.  Maybe some sort
     * of hash function would do.  For now I am writing this in linear time to just work for testing,
     * and maybe sometime in the future we can optimize it (I can think of a way to get it to logorithmic time).
     * Reguardless, I feel like it should be possible in constant time so if you're reading this and are
     * bored then I would be forever grateful if you could make this fast.
     */

    if((uint8_t*)block < (uint8_t*)HEAP_START || (uint8_t*)block > ((uint8_t*)HEAP_START) + TOTAL_HEAP_SIZE){ // out of range
        return -1;
    }

    uint8_t* blk = (uint8_t*) block;

    uint32_t i = 0;
    uint32_t pool_left = POOL_SIZES[i];

    uint32_t index = -1;

    while(blk >= (uint8_t*)HEAP_START){
        if(pool_left <= 0){
            pool_left = POOL_SIZES[i++];
        }

        blk -= POOL_BLOCK_SIZES[i];
        pool_left -= 1;
        index++;
    }

    return index;
}

/**
 * Internal function
 *
 * Gets the index of the pool that a given block of memory is in.
 * For example, if you passed in HEAP_START as block, this function would return
 * 0; since the block at HEAP_START is always in the first pool
 *
 */
uint32_t get_pool(void* block){

    if((uint8_t*)block < (uint8_t*)HEAP_START || (uint8_t*)block > ((uint8_t*)HEAP_START) + TOTAL_HEAP_SIZE){ // out of range
        return -1;
    }

    uint8_t* blk = (uint8_t*) block;

    uint32_t i = 0;
    uint32_t pool_left = POOL_SIZES[i];

    while(blk >= (uint8_t*)HEAP_START){
        if(pool_left <= 0){
            i++;
            pool_left = POOL_SIZES[i];
        }
        blk -= POOL_BLOCK_SIZES[i];
        pool_left -= 1;
    }

    return i;
}



/**
 * Usage: Call with given parameters, check for success or failure to make sure it was
 * configured correctly
 *
 * @return 1 for success, -1 for failure
 */

uint32_t init_heap() {

    uint32_t s = 0;
    for(int i = 0; i < NUMBER_OF_POOLS; i++){
        s += POOL_BLOCK_SIZES[i] * POOL_SIZES[i];
    }

    if (s != TOTAL_HEAP_SIZE) {
        return -1; // sum(pool_sizes * pool_counts) != tota_pool_size, configuration error
    }

    void* start = HEAP_START;
    for(int i = 0; i < NUMBER_OF_POOLS; i++){
        // build linked list for each pool
        pool_heads[i] = build_pool(
            start,
            POOL_BLOCK_SIZES[i],
            POOL_SIZES[i],
            0
            );

        start += POOL_BLOCK_SIZES[i] * POOL_SIZES[i]; // block size (bytes) * amount of blocks
    }

    for(int i = 0; i < IS_FREE_SIZE; i++){
        is_free[i] = 255;
    }

    return 1;
}

/**
 *
 * @param size
 * @return
 */
void* alloc(uint32_t size) {
    // find ideal i
    uint32_t i = 0;
    for(; size > POOL_BLOCK_SIZES[i] && i < NUMBER_OF_POOLS; i++);

    // if the pool for ideal i is already full (null head), keep going to next block until we find a free one
    struct block_t* block = pool_heads[i];
    while(block == ((void*)0) && i < NUMBER_OF_POOLS){
        i++;
        block = pool_heads[i];
    }

    if(block == ((void*)0)){
        return ((void*)0); // no space for a new block of this size
    }

    // update free blocks
    uint32_t index = get_index(block);
    uint32_t big_index = index / 8;
    uint32_t small_index = index % 8;

    // update linked list
    pool_heads[i] = block->next_block;

    is_free[big_index] &= ~((uint8_t)1 << small_index);

    for(int j = 0; j < POOL_BLOCK_SIZES[i]; j++){ // zero this block before returning
        *(((uint8_t*)block) + i) = 0;
    }

    return block;
}

/**
 *
 * @param mem
 */
void free(void* mem) {

    uint32_t i = get_pool(mem);
    if(i == -1){
        return; // bad call, already "free" since it's not in heap
    }

    struct block_t new_head = (struct block_t){
        pool_heads[i]
    };

    *((struct block_t*)mem) = new_head;

    pool_heads[i] = (struct block_t*)mem;

    uint32_t index = get_index(mem);
    uint32_t big_index = index / 8;
    uint32_t small_index = index % 8;

    is_free[big_index] |= (uint8_t)1 << small_index;
}

/**
 *
 * @param mem
 * @return
 */
bool isFree(void* mem) {

    uint32_t index = get_index(mem);

    if(index == -1){
        return false;
    }
    uint32_t big_index = index / 8;
    uint32_t small_index = index % 8;

    return (is_free[big_index] & (1 << small_index)) != 0;
}

