#include <sys/mman.h>

#include <stdio.h>

#include "alloc.h"


int main(){

    void* pool_start = mmap(
        NULL,
        2048,
        PROT_READ | PROT_WRITE,
        MAP_ANONYMOUS | MAP_PRIVATE,
        -1,
        0
    );

    void* general_start = mmap(
        NULL,
        2048,
        PROT_READ | PROT_WRITE,
        MAP_ANONYMOUS | MAP_PRIVATE,
        -1,
        0
    );

    init_heap(pool_start, general_start);

    struct alloc_result mem[2080];

    for(int i = 0; i < 2080; i++){
        mem[i] = alloc(1);
    }

    for(int i = 0; i < 2080; i++){
        free(mem[i].mem);
    }

    for(int i = 0; i < 2080; i++){
        printf("( %i + %i )", isFree(mem[i].mem), mem[i].pool);
    }



    return 0;
}