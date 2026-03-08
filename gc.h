#pragma once 
#include <stddef.h>

typedef struct root{
    void* adress;
    struct root* next;
}root_t;
