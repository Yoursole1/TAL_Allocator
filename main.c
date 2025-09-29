//
// Created by Joshua Beard on 9/27/25.
//

#include <sys/mman.h>

#include <stdio.h>
#include "alloc.c"


int main() {
    HEAP_START = mmap( // HEAP_START variable coming from alloc.c include
        NULL,
        2048,
        PROT_READ | PROT_WRITE,
        MAP_ANONYMOUS | MAP_PRIVATE,
        -1,
        0
    );

    uint32_t success = init_heap();
    if (success != 1) {
        printf("Configuration issue\n");
    }

    uint8_t* mem = (uint8_t*)alloc(1);
    uint8_t* mem2 = (uint8_t*)alloc(1);

    printf("%p\n", HEAP_START);
    printf("%p\n", mem);
    printf("%p\n", mem2);

    printf("%i\n", get_index(mem+64));
    printf("%i\n", get_index(mem2));

    return 0;
}