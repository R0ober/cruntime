#include "slab.h"
#include "common/bump.h"

slab_t slabs[SLAB_NUMBER_OF_SLAB_CLASSES]= {0};
static const size_t slab_sizes[SLAB_NUMBER_OF_SLAB_CLASSES] = SLAB_SIZES;

void slab_init() {
    for(int i=0; i < SLAB_NUMBER_OF_SLAB_CLASSES; i++) {
        slabs[i].size = slab_sizes[i];
        slabs[i].free_list = NULL;
    }
}

void* slab_alloc(size_t size){
    if(size ==0 ) {return NULL;}
    if (slabs[0].size ==0) {slab_init();}
    // find slab size thats smallest and fits size 
    int selected_slab_size_index = -1;
    for (int i=0; i<SLAB_NUMBER_OF_SLAB_CLASSES; i++) {
        if(size <= slabs[i].size) {
            selected_slab_size_index = i;
            break;
        }
    }
    // size doesnt fit in any slab return null
    if(selected_slab_size_index == -1) {
        return NULL;
    }

    //allocate 
    if (slabs[selected_slab_size_index].free_list == NULL) {
        return bump_allocator(slabs[selected_slab_size_index].size);
    }
    // there are actually nodes on the slab
    void* allocated = (void*)(slabs[selected_slab_size_index].free_list); // first avaliable block
    slab_node_t* new_head_node = slabs[selected_slab_size_index].free_list->next;
    slabs[selected_slab_size_index].free_list = new_head_node;
    return allocated;
}

#ifdef TESTING
void slab_reset() {
    for (int i = 0; i < SLAB_NUMBER_OF_SLAB_CLASSES; i++) {
        slabs[i].size = 0;
        slabs[i].free_list = NULL;
    }
}
#endif

int slab_free(void* ptr, size_t size) {
    // checks
    if(ptr== NULL) {return -1;}
    if(size == 0) {return -1;}
    if(slabs[0].size == 0) {return -1;}  // might add so eror handling here? return (void)-1 ?? 
    
    // find correct slab 
    for (int i=0; i<SLAB_NUMBER_OF_SLAB_CLASSES; i++) {
        if(size == slabs[i].size) {
            slab_node_t* head = slabs[i].free_list;
            slabs[i].free_list = (slab_node_t*)ptr;
            slabs[i].free_list->next=head;
            return 0;
        }
    }
    return -1;
}
