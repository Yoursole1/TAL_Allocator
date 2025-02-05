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


    block_t* head = pool_heads[4];

    while(head != NULL){
        printf("%p\n", head);
        head = head->next_block;
    }



    return 0;
}