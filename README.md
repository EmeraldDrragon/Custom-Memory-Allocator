# Custom-Memory-Allocator
Custom implementation of the standard C/C++ memory management
functions. It replaces malloc, calloc, free, and C++ operators
new, delete. Focuses on thread safety and fragmentation control.

## Memory Map and The Program Break
In a standard process, the heap starts after the .bss segment
and ends at the program break, beyond which the memory is unmapped.
The heap grows upwards towards stack, by incrementing the program
break. sbrk(n) function increments it by n bytes. brk(addr) function
sets it to a specific address.

## Heap Layout
The allocator treats the heap as a singly linked list of memory blocks, 
where each allocation is user data with a metadata header. THe header 
(called Block in code) contains size, allocation flag, guard value (0xC0DEC0DE) 
and a pointer to the next Block.

## malloc
Follows a First-Fit Linked List allocation strategy with explicit heap 
exapnsion when no block is found. If a block is found, but is significantly 
larger than requested, it is split into an allocated and a free block.
On initial call it requests an initial sbrk call, which returns the previous 
program break address, which becomes the location of the first Block header.
Additionally, it ensures the 16-Byte alignment for returned pointers and checks
for integer overflow of the requested size.

## free
Manages deallocation and fragmentation. Free verifies that the pointer
is within the heap bounds and that the GUARD_VALUE of the block's metadata
is intact. It merges the freed block with adjacent free blocks forward and
backwards, preventing fragmentation. If the entire heap becomes empty after 
a free, it calls brk to return all memory to the OS.

## calloc, new, delete
C-style calloc is essentially a wrapper function for malloc call, it checks 
for integer overflow, calculates the size, calls malloc, and uses memset to 
set the data to 0.C++ operators new and delete are wrappers for respectfully 
malloc and free, but with a change that it triggers an exception on error.

## Thread safety
The custom allocator features a thread safety mechanism utilizing pthread mutexes.
The lock is put over a shared heap (heap is one for the multiple threads),
protecting the read and write access. Note that I decided for free()
and delete operator to use free_no_lock() function wrapped in the lock to avoid code
duplication and double locking, since delete operator needs to do extensive error handling
by reading from heap.

## Future plans
 * Block search optimization
 * Thread access optimization
 * Expansion on security measures