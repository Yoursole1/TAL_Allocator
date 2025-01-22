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
    void* test = malloc(8);

    if(test == NULL){
        printf("Uh oh, heap didn't do the thing");
    }


    return 0;
}