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

    void* mem = alloc(500);

    printf("%i\n", isFree(mem));

    free(mem);

    printf("%i\n", isFree(mem));


    return 0;
}