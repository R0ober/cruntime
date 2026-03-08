#include "gc.h"
#include "slab.h"

root_t* root_list = NULL;

int gc_register_root(void* ptr ) {
    void* allocated = slab_alloc(sizeof(root_t));
    // allocation failed;
    if(allocated == NULL) {return -1;}
    root_t* root = (root_t*)allocated;
    root_t* next = root_list;
    root->next = next;
    root->adress = ptr;
    root_list = root;
return 0;
}
