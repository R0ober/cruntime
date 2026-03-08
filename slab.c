#include "slab.h"
#include "common/bump.h"
#include "visual.h"
#include <stdio.h>

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

void slab_stats() {
    const char *title = " SLABS ";
    int title_len = 7;
    int frame_width = 78;

    // title bar
    int left_pad = (frame_width - title_len) / 2;
    int right_pad = frame_width - title_len - left_pad;
    printf(COLOR_BORDER BOX_TL RESET);
    for (int i = 0; i < left_pad; i++) printf(COLOR_BORDER BOX_H RESET);
    printf(COLOR_BORDER "%s" RESET, title);
    for (int i = 0; i < right_pad; i++) printf(COLOR_BORDER BOX_H RESET);
    printf(COLOR_BORDER BOX_TR RESET "\n");

    // header row
    printf(COLOR_BORDER BOX_V RESET);
    printf(COLOR_GOLD "  %-8s %-8s %-18s %-39s" RESET, "Size", "Free", "Head", "Free List");
    printf(COLOR_BORDER BOX_V RESET "\n");

    printf(COLOR_BORDER BOX_LT RESET);
    for (int i = 0; i < frame_width; i++) printf(COLOR_BORDER BOX_H RESET);
    printf(COLOR_BORDER BOX_RT RESET "\n");

    size_t total_free = 0;
    int max_bar = 39;

    for (int i = 0; i < SLAB_NUMBER_OF_SLAB_CLASSES; i++) {
        // walk free list
        int count = 0;
        slab_node_t *node = slabs[i].free_list;
        while (node) {
            count++;
            node = node->next;
        }
        total_free += count * slabs[i].size;

        // draw row
        printf(COLOR_BORDER BOX_V RESET);
        printf(COLOR_WHITE "  %-8zu " RESET, slabs[i].size);
        printf(COLOR_WHITE "%-8d " RESET, count);
        if (slabs[i].free_list)
            printf(COLOR_ADDR "%-18p " RESET, (void*)slabs[i].free_list);
        else
            printf(COLOR_ADDR "%-18s " RESET, "(nil)");

        // bar: each char = 1 free node, capped at max_bar
        int overflow = count > max_bar;
        int bar_len = overflow ? max_bar - 1 : count;
        for (int j = 0; j < bar_len; j++) {
            if (j % 2 == 0)
                printf(COLOR_FREE BLOCK_FULL RESET);
            else
                printf(COLOR_FREE BLOCK_EMPTY RESET);
        }
        if (overflow)
            printf(COLOR_GOLD "+" RESET);

        // pad remaining
        int pad = max_bar - bar_len - (overflow ? 1 : 0);
        for (int j = 0; j < pad; j++) printf(" ");

        printf(COLOR_BORDER BOX_V RESET "\n");
    }

    // summary
    printf(COLOR_BORDER BOX_LT RESET);
    for (int i = 0; i < frame_width; i++) printf(COLOR_BORDER BOX_H RESET);
    printf(COLOR_BORDER BOX_RT RESET "\n");

    printf(COLOR_BORDER BOX_V RESET);
    char summary_buf[64];
    int summary_len = snprintf(summary_buf, sizeof(summary_buf), "  Total free: %zub", total_free);
    printf(COLOR_GOLD "  Total free: " COLOR_GREEN "%zub" RESET, total_free);
    for (int i = summary_len; i < frame_width; i++) printf(" ");
    printf(COLOR_BORDER BOX_V RESET "\n");

    // bottom border
    printf(COLOR_BORDER BOX_BL RESET);
    for (int i = 0; i < frame_width; i++) printf(COLOR_BORDER BOX_H RESET);
    printf(COLOR_BORDER BOX_BR RESET "\n");
}
