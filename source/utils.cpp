#include "customAlloc.h"

extern "C"
{
    void* malloc(size_t size) noexcept
    {
        return customAlloc::custom_malloc(size);
    }

    void* calloc(size_t num_memb, size_t size_each) noexcept
    {
        return customAlloc::custom_calloc(num_memb, size_each);
    }

    void free(void* ptr) noexcept
    {
        customAlloc::custom_free(ptr);
    }
}

void* operator new(size_t size)
{
    return customAlloc::custom_new(size);
}

void operator delete(void *address )
{
  customAlloc::custom_delete(address);
}

void operator delete(void* address, std::size_t size)
{
  (void) size;
  customAlloc::custom_delete(address);
}

void* operator new[] ( size_t size )
{
  return customAlloc::custom_new(size);
}

void operator delete[] ( void* address )
{
  customAlloc::custom_delete(address);
}

void operator delete [](void* address, std::size_t size)
{
  (void) size;
  customAlloc::custom_delete(address);
}