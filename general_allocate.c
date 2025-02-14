#include "stdint.h"
#include "stdbool.h"
#include <stddef.h>

#include "alloc_strategy.h"

#pragma once


#define HEAP_SIZE 2048 // since we are allocating arrays of this size, it must be determined at compile time like this

struct Block
{
        void* block;
        uint32_t size;
        bool free;

        struct Block* next;
        uint32_t indexInLinks;
};

struct General_Allocator{

    struct Alloc_Strategy super;
    
    uint32_t heap_remaining;
    struct Block* links;
    uint8_t* free_indecies;

    uint32_t(*findFirstFree)(void* self);

};

uint32_t findFirstFree(void* self){ // TODO optimize
    struct General_Allocator this = *(struct General_Allocator*)(self);
    for(int i = 0; i < HEAP_SIZE; i++){
        if(this.free_indecies[i] == 0){
            return i;
        }
    }
    return -1; // unreachable if "heap_remaining" works properly
}

bool general_isFree(void* self, void* mem)
{
    struct General_Allocator this = *(struct General_Allocator*)(self);

    struct Block* curr = &this.links[0];
    
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

void* general_alloc(void* self, uint32_t size)
{   
    struct General_Allocator this = *((struct General_Allocator*)(self));
    
    if(size > this.heap_remaining){
        return NULL;
    }
    
    // find first block with sizeOf >= size
    // split block into blocks with sizeOf: size, and remaining
    // connect nodes, return allocated block
    
    struct Block* curr = &(this.links[0]);

    while(curr != NULL){
        if(curr->size >= size && curr->free){
            // Take curr, reduce its size to match "size", then attach the remaining block to the end of curr

            uint32_t oversize = curr->size - size;
            curr->size = size;
            curr->free = false;
            
            if(oversize != 0){

                uint32_t firstFree = this.findFirstFree(self);
                
                this.links[firstFree] = (struct Block){
                    curr->block + size, 
                    oversize,
                    true,

                    curr->next,
                    firstFree
                };

                this.free_indecies[firstFree] = 1;

                curr->next = &(this.links[firstFree]);
            }
            
            this.heap_remaining -= size;
            return curr->block;
        }
        curr = curr->next;
    }

    
    return NULL; // no block could be found, possible fragmentation issue ):
}


void general_free(void* self, void* mem)
{
    struct General_Allocator this = *(struct General_Allocator*)(self);
    if(this.super.isFree(self, mem)){
        return; // already free
    }

    // find first block containing this pointer
    // If free block before or after, merge
    //      otherwise, just flip boolean to free
    // update free_indcies 

    struct Block* prev = NULL;
    struct Block* curr = &this.links[0];
    struct Block* next = curr->next;

    while(curr != NULL){

        void* curr_mem = curr->block;
        uint32_t size = curr->size;

        if(curr_mem <= mem && mem < curr_mem + size){ // mem is within this block
            // merge with free blocks before and after, if they exist
            // make this block marked as free

            curr->free = true;

            uint8_t* p = (uint8_t*)curr->block;
            for(int i = 0; i < curr->size; i++){ // Zero the block we are freeing
                *p++ = 0;
            }

            this.heap_remaining += curr->size; // update heap remaining with the size of freed block

            if(next != NULL && next->free){

                curr->size += next->size;
                curr->next = next->next;
                

                this.free_indecies[next->indexInLinks] = 0;
                // no need to overwrite it in "links", as the index
                // is now considered free and it will be overwritten

                // it won't interfere with iterating either since the 
                // linked list dropped this node
            }

            if(prev != NULL && prev->free){
                prev->size += curr->size;
                prev->next = curr->next;

                this.free_indecies[curr->indexInLinks] = 0;
            }
            
        }
        
        prev = curr; // hold on to prev because we need it when merging blocks
        curr = curr->next;

        if(curr != NULL){ // careful to not deref NULL pointer
            next = curr->next;
        }
        
    }
}

struct Block links[HEAP_SIZE];
uint8_t free_indecies[HEAP_SIZE];

// WARNING: Since links and free_indecies are global, I am 90% sure this method should only be called once
struct General_Allocator build_general_allocator(void* heap_start)
{
    
    struct Alloc_Strategy super = {
        .heap_size = HEAP_SIZE,
        .heap_start = heap_start,

        .isFree = &general_isFree,
        .alloc = &general_alloc,
        .free = &general_free
    };
    
    links[0] = (struct Block){
        heap_start,
        HEAP_SIZE,
        true,

        NULL,
        0
    };

    free_indecies[0] = 1;
    
    struct General_Allocator allocator = {
        .super = super,
        .heap_remaining = HEAP_SIZE,

        .links = links,
        .free_indecies = free_indecies,

        .findFirstFree = &findFirstFree
    };
    
    return allocator;
}
