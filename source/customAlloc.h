#ifndef CUSTOM_ALLOC_H_
#define CUSTOM_ALLOC_H_

#include "unistd.h"
#include "stdlib.h"

class customAlloc
{
private:
    typedef struct block
    {
        size_t size;
        bool is_allocated;
        size_t guard;
        struct block* next;
    } Block;
    static Block* heap_start;
    static void* heap_end;
    static constexpr size_t GUARD_VALUE = 0xC0DEC0DE;
    static pthread_mutex_t heap_mutex;
public:
    static void* custom_malloc(size_t size);
    static void* custom_calloc(size_t num_memb, size_t size_each);
    static void custom_free(void* ptr);

    static void* custom_new(size_t size);
    static void custom_delete(void* ptr);

    static void free_no_lock(void* ptr);
};


#endif