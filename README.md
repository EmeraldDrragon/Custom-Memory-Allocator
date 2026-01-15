# Custom-Memory-Allocator

Custom implementation of the standard C/C++ memory management
functions. It replaces malloc, calloc, free, and C++ operators
new, delete. Focuses on thread safety, fragmentation control 
and defensive programming.

## Memory Map and The Program Break
In a standard process, the heap starts after the .bss segment
and ends at the program break, beyond which the memory is unmapped.
The heap grows upwards towards stack, by incrementing the program
break. sbrk(n) function increments it by n bytes. brk(addr) function
sets it to a specific address.

-- WIP --
 * [x] Name
 * [x] Description
 * [x] Memory Map and The Program Break
 * [ ] How dynamic memory looks like (header->data->header->data)
 * [ ] malloc
 * [ ] free
 * [ ] calloc, new, delete
 * [ ] Thread Safety
 * [ ] Future plans