#pragma once 
#include <stddef.h>

typedef struct root{
    void* adress;
    struct root* next;
}root_t;

typedef struct allocation{
    void* block;
    size_t size;
    int marked;  // 1 for marked
    struct allocation* next;
}allocation_t;

