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

int gc_register_root(void* ptr );
void* gc_alloc(size_t size);
int gc_collect(void); 
void gc_stats(void);
