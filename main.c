#include <sys/mman.h>

#include <stdio.h>
#include "general_allocate.c"



int main(){
    
    void* heap_start = mmap(
        NULL,
        HEAP_SIZE,
        PROT_READ | PROT_WRITE,
        MAP_ANONYMOUS | MAP_PRIVATE,
        -1,
        0
    );

    
    struct General_Allocator allocator = build_general_allocator(heap_start);
    
    void* mem = allocator.super.alloc(&allocator, 1);

    printf("%i\n", allocator.super.isFree(&allocator, mem));
    printf("%i\n", allocator.super.isFree(&allocator, mem + 10));

    allocator.super.free(&allocator, mem);

    printf("%i\n", allocator.super.isFree(&allocator, mem));
    printf("%i\n", allocator.super.isFree(&allocator, mem + 10));
    
    printf("%p\n", heap_start);
    printf("%p\n", mem);



    return 0;
}