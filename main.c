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

    // printf("%p\n", heap_start);

    void* test = alloc(2);
    void* test_2 = alloc(1);
    void* test_3 = alloc(2);
    printHeap();

    free(test);
    free(test_2);
    
    printHeap();

    void* test_4 = alloc(1);

    printHeap();

    void* test_5 = alloc(2);

    printHeap();
    
    for(void* i = heap_start; i < heap_start + 10; i++){
        printf("%i\n", isFree(i));
    }




    return 0;
}