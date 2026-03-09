#include "gc.h"
#include "slab.h"

root_t* root_list = NULL;
allocation_t* allocation_list = NULL;

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

void* gc_alloc(size_t size) {
    // user allocation 
    void* ptr = slab_alloc(size);
    if(ptr == NULL) {return NULL;}
    // allocation to track user allocation
    void* ptr_2 = slab_alloc(sizeof(allocation_t));
    if(ptr_2 == NULL) {
        slab_free(ptr,size);
        return NULL;
    }
    allocation_t* allocated = (allocation_t*)ptr_2;
    allocation_t* next = allocation_list;
    allocated->next = next;
    allocated->block = ptr;
    allocated->marked = 0;
    allocated->size = size;
    allocation_list = allocated;
    return ptr;
}
int gc_collect() {
    // walk the roots
    root_t* root = root_list;
    allocation_t* alloc;
    while(root != NULL) {
        
        alloc = allocation_list;
        while(alloc != NULL) {
            if(*(void**)root->adress == alloc->block) {
                alloc->marked= 1;
            }
        alloc = alloc->next;
        }
        root = root->next;
    }

    // sweep 
    alloc = allocation_list; 
    allocation_t* prev = NULL; 
    int err = 0;  
    while(alloc != NULL) {
        allocation_t* next = alloc->next;
        if(alloc->marked == 0) {
            err = slab_free(alloc->block,alloc->size);
            if(err == -1) {
                    return -1;
            }
            if(prev) {
                prev->next = next;
                err = slab_free(alloc,sizeof(allocation_t));
                if (err == -1) {
                    prev->next = alloc;
                    alloc->next = next;
                    return -1;
                }
            } else {  /// first block 
                allocation_list = next;
                err = slab_free(alloc,sizeof(allocation_t));
                if (err == -1) {
                    allocation_list = alloc;
                    alloc->next = next;
                    return -1;
                }
            }
        }
        prev = alloc;
        alloc = next;
    }

    // reset
    alloc = allocation_list;
    while(alloc != NULL) {
        alloc->marked = 0;
        alloc = alloc->next;
    }
    return 0;


}
