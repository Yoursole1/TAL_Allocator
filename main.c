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

    uint16_t* mem = alloc(NULL, 17);
    printf("------------------------%i\n", isFree(NULL, mem));

    block_t* head = pool_heads[0];

    while(head != NULL){
        printf("%i\n", get_index(head));
        head = head->next_block;
    }

    free(NULL, mem);
    printf("------------------------%i\n", isFree(NULL, mem));

    head = pool_heads[0];

    while(head != NULL){
        printf("%i\n", get_index(head));
        head = head->next_block;
    }



    return 0;
}