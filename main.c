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

    struct General_Allocator alloca = build_general_allocator(heap_start);

    void* mem[30];

    for(int i = 0; i < 30; i++){
        mem[i] = alloca.super.alloc(&alloca, 8);
    }

    for(int i = 0; i < 30; i++){
        printf("%i\n" ,alloca.super.isFree(&alloca, mem[i]));
    }
    printf("---------------\n");
    for(int i = 0; i < 30; i += 2){
        alloca.super.free(&alloca, mem[i]);
    }

    for(int i = 0; i < 30; i++){
        printf("%i\n" ,alloca.super.isFree(&alloca, mem[i]));
    }


    return 0;
}