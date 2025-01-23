#include <sys/mman.h>
#include "allocate.c"

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

    init_heap();

    printf("%p\n", heap_start);

    void* test = malloc(1);

    // void* test_2 = malloc(1);
    
    printf("%p\n", test);
    printf("%p\n", heap);




    return 0;
}