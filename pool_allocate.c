

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include <stdio.h>



// SETTINGS ------------------------- 

#define HEAP_SIZE 2048 // Tile game be like 
// CHANGE, currently being set up in main, use macro
void* pool_start = (void*)0; 
void* general_start = (void*)0; 


// ----------------------------------

#define MAX_POOL_SIZE 256 // actually ends up being 2^n - 1 for all these sizes because first byte 
#define EXP 8 // 2^EXP = MAX_POOL_SIZE, defined for ease of use

uint32_t pool_sizes[EXP]  = {20,20,10,8,8,5,5,5}; // 20*1+20*2+10*4+8*8+8*16+5*32+5*64+5*128 = 1412 bytes used for pool

void init_heap()
{
    // pooled space = HEAPSIZE / 2
    int s = 0; // s = 1412 with current config
    for(int i = 0; i < EXP; i++){
        s += (1 << i) * pool_sizes[i];
    }
    
    general_start = pool_start + s;
    
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