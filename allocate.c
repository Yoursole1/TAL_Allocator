#include "stdint.h"
#include "stdbool.h"
#include <stddef.h>
#include <stdio.h>

#define HEAP_SIZE 1024

// CHANGE, currently being set up in main, use macro
void* heap_start = (void*)0;

uint32_t heap_remaining = HEAP_SIZE;

struct Block{
    void* block;
    uint32_t size; 
    bool free;

    struct Block* next;
};

struct Block* heap;

void init_heap(){
    heap = &((struct Block){
        heap_start,
        HEAP_SIZE,
        true,

        NULL
    });
}

bool isFree(void* mem)
{
    struct Block* curr = heap;

    while(curr != NULL){
        void* curr_mem = curr->block;
        uint32_t size = curr->size;

        if(curr_mem <= mem && mem < curr_mem + size){ // mem is within this block
            return curr->free;
        }

        curr = curr->next;
    }

    return false; // mem is not within the heap (bad call)
}

void* malloc(uint32_t size)
{
    if(size > heap_remaining){
        return NULL;
    }
    
    // find first block with sizeOf >= size
    // split block into blocks with sizeOf: size, and remaining
    // connect nodes, return allocated block
    
    struct Block* curr = heap;
    
    while(curr != NULL){
        if(curr->size >= size && curr->free){
            // Take curr, reduce its size to match "size", then attach the remaining block to the end of curr

            uint32_t oversize = curr->size - size;
            curr->size = size;
            curr->free = false;

            
            if(oversize != 0){
                struct Block remaining = {
                curr->block + size, 
                oversize,
                true,

                curr->next
                };


                curr->next = &remaining;
            }
            
            heap_remaining -= size;
            return curr->block;
        }

        curr = curr->next;
    }

    return NULL; // no block could be found, possible fragmentation issue ):
}


void free(void* mem)
{
    if(isFree(mem)){
        return; // already free
    }

    // impl
}