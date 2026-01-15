#include <iostream>
#include <cstdlib>
#include <vector>

int main()
{
    std::cout << "[C-style malloc]" << std::endl;
    int* malloc_ptr = (int*)malloc(sizeof(int));
    if(malloc_ptr == NULL)
    {
        std::cout << "[C-style malloc] allocation failed" << std::endl;
        return -1;
    }
    *malloc_ptr = 100;
    std::cout << "[C-style malloc] pointer value: " << *malloc_ptr << std::endl;


    std::cout << "[C-style calloc]" << std::endl;
    int* calloc_ptr = (int*)calloc(10, sizeof(int));
    if(calloc_ptr == NULL)
    {
        std::cout << "[C-style calloc] allocation failed" << std::endl;
        return -1;
    }
    calloc_ptr[7] = 200;
    std::cout << "[C-style calloc] pointer value: " << calloc_ptr[7] << std::endl;


    std::cout << "[C-style free]" << std::endl;
    free(malloc_ptr);
    free(calloc_ptr);
    std::cout << "[C-style free] malloc'd and calloc'd variables are free'd" << std::endl;


    std::cout << "[Operator new]" << std::endl;
    int* new_ptr;
    try 
    {
        new_ptr = new int(300);
    } 
    catch(std::bad_alloc& e)
    {
        std::cout << "[operator new] allocation failed" << std::endl;
        return -1;
    }
    std::cout << "[operator new] pointer value: " << *new_ptr << std::endl;


    std::cout << "[Operator delete]" << std::endl;
    delete new_ptr;
    std::cout << "[operator delete] allocated ptr is deleted" << std::endl;

    return 0;
}