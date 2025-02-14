#include "general_allocate.c"
#include "pool_allocate.c"

#include "alloc.h"


struct Pool_Allocator pool_allocator;
struct General_Allocator general_allocator;

void init_heap(void* pool_start, void* general_start)
{
    pool_allocator = build_pool_allocator(pool_start);
    general_allocator = build_general_allocator(general_start);
}

bool isFree(void* mem)
{
    return 
        pool_allocator.super.isFree(&pool_allocator, mem) || 
        general_allocator.super.isFree(&general_allocator, mem);
}


void* alloc(uint32_t size)
{
    if(size > MAX_POOL_SIZE) {
        return general_allocator.super.alloc(&general_allocator, size);
    }

    void* mem = pool_allocator.super.alloc(&pool_allocator, size);

    if(mem == NULL){
        return general_allocator.super.alloc(&general_allocator, size);
    }
}


void free(void* mem)
{
    if(mem >= pool_allocator.super.heap_start && mem < pool_allocator.super.heap_start + pool_allocator.super.heap_size){
        pool_allocator.super.free(&pool_allocator, mem);
    }else{
        general_allocator.super.free(&general_allocator, mem);
    }
}
