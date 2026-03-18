#include "gc.h"
#include "slab.h"
#include "visual.h"
#include <stdio.h>
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
            if(root->adress == alloc->block) {
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
#ifndef USE_EMBEDDED
void gc_stats(void) {
    const int frame_width = 78;
    const int inset = 2;
    size_t root_count = 0;
    size_t root_bytes = 0;
    size_t allocation_count = 0;
    size_t live_bytes = 0;
    size_t marked_count = 0;
    size_t unmarked_count = 0;
    size_t allocation_meta_bytes = 0;

    root_t* root = root_list;
    while (root != NULL) {
        root_count++;
        root = root->next;
    }
    root_bytes = root_count * sizeof(root_t);

    allocation_t* alloc = allocation_list;
    while (alloc != NULL) {
        allocation_count++;
        live_bytes += alloc->size;
        if (alloc->marked != 0) {
            marked_count++;
        } else {
            unmarked_count++;
        }
        alloc = alloc->next;
    }
    allocation_meta_bytes = allocation_count * sizeof(allocation_t);

    char size_buf[32];
    visual_draw_title(" GC Overview ", frame_width);

    visual_draw_subbox_top(" Roots ", frame_width, inset);
    visual_draw_subbox_kv_row("Registered roots", root_count, frame_width, inset);
    visual_draw_subbox_kv_row("Root metadata bytes", root_bytes, frame_width, inset);
    visual_draw_subbox_bottom(frame_width, inset);

    visual_draw_subbox_top(" Allocator ", frame_width, inset);
    visual_draw_subbox_kv_row("Tracked allocations", allocation_count, frame_width, inset);
    visual_draw_subbox_kv_row("Marked allocations", marked_count, frame_width, inset);
    visual_draw_subbox_kv_row("Unmarked allocations", unmarked_count, frame_width, inset);
    visual_draw_subbox_kv_row("Live bytes", live_bytes, frame_width, inset);
    visual_draw_subbox_kv_row("Allocation metadata bytes", allocation_meta_bytes, frame_width, inset);

    visual_draw_subbox_divider(frame_width, inset);
    visual_draw_subbox_columns_header4("Block", "Size", "Marked", "",22, 10, 8, 0,frame_width, inset);
    alloc = allocation_list;
    if (alloc == NULL) {
        visual_draw_subbox_columns_row4("(empty)", "-", "-", "",2, 10, 8, 0,frame_width, inset);
    }
    while (alloc != NULL) {
        char addr_buf[32];
        const char* marked_text = alloc->marked ? "yes" : "no";
        snprintf(addr_buf, sizeof(addr_buf), "%p", alloc->block);
        snprintf(size_buf, sizeof(size_buf), "%zu", alloc->size);

        visual_draw_subbox_columns_row4(addr_buf, size_buf, marked_text, "",22, 10, 8, 0,frame_width, inset);

        alloc = alloc->next;
    }
    visual_draw_subbox_bottom(frame_width, inset);
    visual_draw_horizontal(BOX_BL, BOX_BR, frame_width);
}
#else
void gc_stats(void) {
    // No-op on embedded builds.
}
#endif
