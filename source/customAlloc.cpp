#include <pthread.h>
#include "customAlloc.h"
#include <new>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "string.h"

customAlloc::Block* customAlloc::heap_start = nullptr;
void* customAlloc::heap_end = nullptr;
constexpr size_t CHUNK_SIZE = 3 * 4096;
pthread_mutex_t customAlloc::heap_mutex = PTHREAD_MUTEX_INITIALIZER;

void* customAlloc::custom_malloc(size_t size)
{
    if(size == 0)
    {
        return nullptr;
    }

    size_t aligned_size = (size + 15) & ~15;
    if(aligned_size < size || (aligned_size + sizeof(Block)) < aligned_size)
    {
        return nullptr;
    }
    size_t total_size = sizeof(Block) + aligned_size;
    if(total_size < sizeof(Block))
    {
        return nullptr;
    }
    pthread_mutex_lock(&heap_mutex);

    if(heap_start == nullptr && heap_end == nullptr)
    {
        
        size_t first_alloc = CHUNK_SIZE;
        if(total_size > CHUNK_SIZE)
        {
        first_alloc = total_size;
        } 

        heap_start = (Block*)sbrk(first_alloc);
        if(heap_start == (void*)-1)
        {
        pthread_mutex_unlock(&heap_mutex);
        return nullptr;
        } 

        heap_start->size = first_alloc - sizeof(Block);
        heap_start->is_allocated = false;
        heap_start->guard = GUARD_VALUE;
        heap_start->next = nullptr;
        heap_end = (char*)heap_start + first_alloc;
        // pthread_mutex_unlock(&heap_mutex);
    }

    Block* curr = heap_start;
    Block* prev = nullptr;
    while(curr != nullptr)
    {
        if(!curr->is_allocated && curr->size >= aligned_size)
        {
        if(curr->size >= aligned_size + sizeof(Block) + 16)
        {
            Block* new_block = (Block*)((char*)curr + total_size);
            new_block->size = curr->size - total_size;
            new_block->is_allocated = false;
            new_block->guard = GUARD_VALUE;
            new_block->next = curr->next;
            curr->size = aligned_size;
            curr->next = new_block;
        }
        curr->is_allocated = true;
        curr->guard = GUARD_VALUE;
        char* return_pointer = (char*)curr + sizeof(Block);
        pthread_mutex_unlock(&heap_mutex);
        return return_pointer;
        }
        prev = curr;
        curr = curr->next;
    }


    size_t alloc_size = CHUNK_SIZE;
    if(total_size > CHUNK_SIZE) 
    {
        alloc_size = total_size;
    }
    void* new_memory = sbrk(alloc_size);
    if(new_memory == (void*)-1) 
    {
        pthread_mutex_unlock(&heap_mutex);
        return nullptr;
    }

    Block* new_block = (Block*)new_memory;
    new_block->size = alloc_size - sizeof(Block);
    new_block->is_allocated = true;
    new_block->guard = GUARD_VALUE;
    new_block->next = nullptr;

    if(alloc_size > aligned_size + sizeof(Block) + 16)
    {
        Block* free_block = (Block*)((char*)new_block + total_size);
        free_block->size = alloc_size - total_size - sizeof(Block);
        free_block->is_allocated = false;
        free_block->guard = GUARD_VALUE;
        free_block->next = nullptr;
        new_block->size = aligned_size;
        new_block->next = free_block;
    }

    if(prev != nullptr)
    {
        prev->next = new_block;
    }
    heap_end = (char*)new_memory + alloc_size;

    char* return_pointer = (char*)new_block + sizeof(Block);
    pthread_mutex_unlock(&heap_mutex);
    return return_pointer;
}

void customAlloc::free_no_lock(void* ptr)
{
    if(heap_start == nullptr && heap_end == nullptr)
    {
        return;
    }
    
    Block* block = (Block*)((char*)ptr - sizeof(Block));
    if(heap_start == nullptr ||
        (char*)ptr < (char*)heap_start + sizeof(Block) ||
        (char*)ptr > (char*)heap_end)
    {
        exit(-1);
    }
    
    Block *curr = heap_start;
    Block *prev = nullptr;
    bool found = false;
    while(curr != nullptr)
    {
        if(curr == block)
        {
        if(!curr->is_allocated || curr->guard  != GUARD_VALUE)
        {
            exit(-1);
        }
        found = true;
        break;
        }
        prev = curr;
        curr = curr->next;
    }
    if(!found)
    {
        exit(-1);
    }

    curr->is_allocated = false;

    if(curr->next != nullptr && 
        (char*)curr + sizeof(Block) + curr->size == (char*)curr->next &&
        !curr->next->is_allocated &&
        curr->next->guard == GUARD_VALUE)
    {
        curr->size += curr->next->size + sizeof(Block);
        curr->next = curr->next->next;
    }

        if(prev != nullptr && 
        (char*)prev + sizeof(Block) + prev->size == (char*)curr &&
        !prev->is_allocated &&
        prev->guard == GUARD_VALUE)
    {
        prev->size += curr->size + sizeof(Block);
        prev->next = curr->next;
        curr = prev;
    }

    curr->guard = GUARD_VALUE;

    bool all_free = true;
    curr = heap_start;
    while(curr != nullptr)
    {
        if(curr->is_allocated)
        {
        all_free = false;
        break;
        }
        curr = curr->next;
    }
    if(all_free && heap_start != nullptr)
    {
        if(brk(heap_start) == -1)
        {
        exit(-1);
        }
        heap_start = nullptr;
        heap_end = nullptr;
        return;
    }
}

void customAlloc::custom_free(void* ptr)
{
    if(ptr == nullptr)
    {
        return;
    }

    pthread_mutex_lock(&heap_mutex);
    free_no_lock(ptr);
    pthread_mutex_unlock(&heap_mutex);
}

void* customAlloc::custom_calloc(size_t num_memb, size_t size_each)
{
if(num_memb == 0 || size_each == 0)
  {
    return nullptr;
  }
  if(num_memb != 0 && size_each > SIZE_MAX / num_memb)
  {
    return nullptr;
  }
  size_t total_size = num_memb * size_each;
  void* ptr = custom_malloc(total_size);
  if(ptr != nullptr)
  {
    memset(ptr, 0, total_size);
  }
  return ptr;
}

void* customAlloc::custom_new(size_t size)
{
    void *ptr = custom_malloc(size);
    if(ptr == nullptr)
    {
        throw std::bad_alloc();
    }
    return ptr;
}

void customAlloc::custom_delete(void* p)
{
    if(p == nullptr)
    {
        return;
    }
    
    pthread_mutex_lock(&heap_mutex);

    if(heap_start == nullptr && heap_end == nullptr)
    {
        pthread_mutex_unlock(&heap_mutex);
        return;
    }
    Block* block = (Block*)((char*)p - sizeof(Block));

    if(heap_start == nullptr ||
        (char*)p < (char*)heap_start + sizeof(Block) ||
        (char*)p > (char*)heap_end)
    {
        pthread_mutex_unlock(&heap_mutex);
        throw std::bad_alloc();
    }

    Block* curr = heap_start;
    bool found = false;
    while(curr != nullptr)
    {
        if(curr == block)
        {
        if(!curr->is_allocated || curr->guard != GUARD_VALUE)
        {
            pthread_mutex_unlock(&heap_mutex);
            throw std::bad_alloc();
        }
        found = true;
        break;
        }
        curr = curr->next;
    }
    if(!found)
    {
        pthread_mutex_unlock(&heap_mutex);
        throw std::bad_alloc();
    }
    free_no_lock(p);
    pthread_mutex_unlock(&heap_mutex);
}
