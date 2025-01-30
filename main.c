#include <sys/mman.h>
#include "coolalloc.c" // gotta expose the impl here so that we can write to heap_start.

#include <stdio.h>



int main(){
    
    heap_start = mmap(
        NULL,
        HEAP_SIZE,
        PROT_READ | PROT_WRITE,
        MAP_ANONYMOUS | MAP_PRIVATE,
        -1,
        0
    );

    // printf("%p\n", heap_start);

    void* test = alloc(2);

    printf("%i", int_log(9));


    return 0;
}