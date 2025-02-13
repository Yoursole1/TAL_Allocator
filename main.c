#include <sys/mman.h>

#include <stdio.h>
#include "pool_allocate.c"



int main(){
    
    heap_start = mmap(
        NULL,
        HEAP_SIZE,
        PROT_READ | PROT_WRITE,
        MAP_ANONYMOUS | MAP_PRIVATE,
        -1,
        0
    );

    printf("heap start: %p\n", heap_start);

    init_heap();

    void* mem[32];

    for(int i = 0; i < 32; i++){
        mem[i] = alloc(NULL, 8);
    }

    for(int i = 0; i < 32; i++){
        printf("Pool (%i): %i\n", i, get_pool(mem[i]));
    }


    return 0;
}