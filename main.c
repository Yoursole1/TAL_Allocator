#include <sys/mman.h>

#include <stdio.h>
#include "pool_allocate.c"



int main(){
    
    void* heap_start = mmap(
        NULL,
        HEAP_SIZE,
        PROT_READ | PROT_WRITE,
        MAP_ANONYMOUS | MAP_PRIVATE,
        -1,
        0
    );

    
    init_heap();



    return 0;
}