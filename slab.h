#pragma once
#include <stddef.h>


#define SLAB_NUMBER_OF_SLAB_CLASSES 7
#define SLAB_SIZES {8,16,32,64,128,256,512}

typedef struct slab{
    size_t size;
    struct slab_node* free_list;
}slab_t;

typedef struct slab_node {
   struct slab_node* next;

}slab_node_t;

int slab_free(void* ptr, size_t size);
void* slab_alloc(size_t size);

#ifdef TESTING
void slab_init();
void slab_reset();
extern slab_t slabs[SLAB_NUMBER_OF_SLAB_CLASSES];
#endif 
