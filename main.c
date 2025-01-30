#include <sys/mman.h>
#include "allocate.c"

#include <stdio.h>

void printHeap(){ // todo remove
    printf("--------------------\n");
    printf("Base Address: %p\n", heap_start);
    printf("Heap Used: %i\n", HEAP_SIZE - heap_remaining);
    struct Block* curr = &links[0];

    while(curr != NULL){

        printf("%p (Free: %i, Size: %i) -> ", curr->block, curr->free, curr->size);
        
        curr = curr->next;
    }

    printf("NULL\n");

    printf("[");
    for(int i = 0; i < 10; i++){
        printf("%i, ", free_indecies[i]);
    }
    printf("]\n");

    printf("--------------------\n");
}

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

    free(test_5);

    printHeap();

    free(test_3);

    printHeap();

    free(test_4);

    printHeap();

    
    for(void* i = heap_start; i < heap_start + 10; i++){
        printf("%i\n", isFree(i));
    }




    return 0;
}