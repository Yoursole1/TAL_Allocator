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

    void* mem[50];

    for(int i = 0; i < 50; i++){
        mem[i] = alloc(8);
    }

    for(int i = 0; i < 50; i++){
        free(mem[i]);
    }

    for(int i = 0; i < 50; i++){
        printf("%i\n", isFree(mem[i]));
    }



    return 0;
}