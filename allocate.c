#include "stdint.h"
#include "stdbool.h"
#include <stddef.h>

#define HEAP_SIZE 1024

// CHANGE, currently being set up in main, use macro
void* heap_start = (void*)0;

uint32_t heap_remaining = HEAP_SIZE;

struct Block{
    void* block;
    uint32_t size; 
    bool used;

    struct Block* next;
};

struct Block heap;

void init_heap(){
    heap = (struct Block){
        heap_start,
        HEAP_SIZE,
        false,

        NULL
    };
}

void* malloc(uint8_t size)
{
    if(size > heap_remaining){
        return NULL;
    }

    // impl

    return NULL;
}


void free(void* mem)
{
    if(isFree(mem)){
        return; // already free
    }

    // impl
}


bool isFree(void* mem)
{
    struct Block* curr = &heap;

    while(curr != NULL){
        void* curr_mem = (*curr).block;
        uint32_t size = (*curr).size;

        if(curr_mem < mem && mem < curr_mem + size){ // mem is within this block
            return !(*curr).used;
        }

        curr = (*curr).next;
    }

    return false; // mem is not within the heap (bad call)
}