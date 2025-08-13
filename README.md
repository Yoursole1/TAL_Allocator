Custom memory allocator written for the UW SARP flight computer.  Combines both pool allocation and a general allocator that partitions the memory into custom size blocks.


To run: 
gcc -o alloc_bin alloc.c main.c
./alloc_bin

The reason we only build alloc.c and main.c is because alloc.c includes general_allocate.c and pool_allocate.c (which is bad, should be changed) so including them here makes duplicate function definitions.  Weird issue, it took me like 2 hours to fix, I swear it didn't happen before, but alas...here we are, so if you're having this issue in the future somehow, there you go
