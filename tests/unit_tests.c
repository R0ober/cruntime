#include <stdio.h>
#include <string.h>
#include "../slab.h"
#include "unit.h"
#include "../common/bump.h"
#include "../gc.h"

extern slab_t slabs[SLAB_NUMBER_OF_SLAB_CLASSES];
static const size_t slab_sizes[SLAB_NUMBER_OF_SLAB_CLASSES] = SLAB_SIZES;
extern root_t* root_list;
extern allocation_t* allocation_list;

void test_slab_init() {
 
    ASSERT_EQ(slabs[0].size, 0);
    ASSERT_EQ(slabs[0].free_list, NULL);
    slab_init();
    
    ASSERT_NOT_EQ(slabs[0].size,0);

    for( int i = 0; i < SLAB_NUMBER_OF_SLAB_CLASSES; i++) {
        ASSERT_EQ(slabs[i].size,slab_sizes[i]);
    }

}
void test_slab_alloc_no_nodes_on_slab() {
    slab_reset();
    ASSERT_EQ(slabs[0].size, 0);
    ASSERT_EQ(slabs[0].free_list, NULL);

    // check invalid input 
    void* out = slab_alloc(0);
    ASSERT_EQ(out,NULL);

    // check lazy init 
    out = slab_alloc(slab_sizes[0]);
    ASSERT_NOT_EQ(out,NULL);
    for( int i = 0; i < SLAB_NUMBER_OF_SLAB_CLASSES; i++) {
        ASSERT_EQ(slabs[i].size,slab_sizes[i]);
    }

    // check allocation of too large block 
    out = slab_alloc(slab_sizes[SLAB_NUMBER_OF_SLAB_CLASSES-1]+1);
    ASSERT_EQ(out,NULL);
}

void test_slab_free_invalid_inputs() {
    slab_reset();
    
    int err = slab_free(NULL,1);
    ASSERT_EQ(err,-1);
    void* a = slab_alloc(slab_sizes[0]);
    err = slab_free(a,1);
    ASSERT_EQ(err,-1);
}

void test_slab_free_slabs_not_inited() {
    slab_reset();
    void* a = slab_alloc(slab_sizes[0]);
    slab_reset();
    int err = slab_free(a,1);
    ASSERT_EQ(err,-1);
}

void test_slab_free_no_existing_free_list() {
    slab_reset();
    void* a = slab_alloc(slab_sizes[0]);
    int err = slab_free(a,slab_sizes[0]);
    ASSERT_EQ(err,0);
    ASSERT_EQ(a,slabs[0].free_list);
}

void test_slab_free_existing_free_list() {
    slab_reset();
    void* a = slab_alloc(slab_sizes[0]);
    void* a2 = slab_alloc(slab_sizes[0]);
    int err = slab_free(a,slab_sizes[0]);
    err = slab_free(a2,slab_sizes[0]);
    ASSERT_EQ(err,0);
    ASSERT_EQ(a2,slabs[0].free_list);
    ASSERT_EQ(a,slabs[0].free_list->next);
}

void test_slab_alloc_nodes_exist_on_slab() {
    slab_reset();
    void* a = slab_alloc(slab_sizes[0]);
    void* a2 = slab_alloc(slab_sizes[0]);
    slab_free(a,slab_sizes[0]);
    slab_free(a2,slab_sizes[0]);

    void* b = slab_alloc(slab_sizes[0]);

    ASSERT_EQ(b,a2);
}

void test_slab_stress() {
    slab_reset();
    slab_init();

    #define ALLOCS_PER_CLASS 50
    void* ptrs[SLAB_NUMBER_OF_SLAB_CLASSES][ALLOCS_PER_CLASS];

    // allocate a bunch per size class
    for (int i = 0; i < SLAB_NUMBER_OF_SLAB_CLASSES; i++) {
        for (int j = 0; j < ALLOCS_PER_CLASS; j++) {
            ptrs[i][j] = slab_alloc(slab_sizes[i]);
            ASSERT_PTR_NOT_NULL(ptrs[i][j]);
        }
    }

    // free them all back
    for (int i = 0; i < SLAB_NUMBER_OF_SLAB_CLASSES; i++) {
        for (int j = 0; j < ALLOCS_PER_CLASS; j++) {
            int err = slab_free(ptrs[i][j], slab_sizes[i]);
            ASSERT_EQ(err, 0);
        }
    }

    // verify free list lengths
    for (int i = 0; i < SLAB_NUMBER_OF_SLAB_CLASSES; i++) {
        int count = 0;
        slab_node_t *node = slabs[i].free_list;
        while (node) { count++; node = node->next; }
        ASSERT_EQ(count, ALLOCS_PER_CLASS);
    }

#ifndef USE_EMBEDDED
    slab_stats();
#endif
}

void test_gc_collect_register_root() {
    slab_reset();
    slab_init();
    ASSERT_EQ(root_list,NULL);
    void* ptr = gc_alloc(8); 
    int err = gc_register_root(ptr);
    ASSERT_EQ(err, 0);
    ASSERT_NOT_EQ(root_list,NULL);
    ASSERT_EQ(root_list->adress,ptr);
    ASSERT_EQ(root_list->next,NULL);
}

void test_gc_collect_unregistered_root() {
    slab_reset();
    slab_init();
    root_list = NULL;
    allocation_list = NULL;
    void* ptr = gc_alloc(8); 
    void* ptr_2 = gc_alloc(8);
    void* ptr_3 = gc_alloc(8); 
    void* ptr_4 = gc_alloc(16);
    int err_2 = gc_register_root(ptr);
    int err_3 = gc_register_root(ptr_2);
    int err_4 = gc_register_root(ptr_3);
    ASSERT_EQ(err_2, 0);
    ASSERT_EQ(err_3, 0);
    ASSERT_EQ(err_4, 0);
    root_t* curr = root_list;    
    ASSERT_EQ((curr)->adress,ptr_3);
    ASSERT_EQ((curr->next)->adress,ptr_2);
    ASSERT_EQ((curr->next->next)->adress,ptr);
    ASSERT_EQ((curr->next->next->next),NULL);
    ASSERT_NOT_EQ(ptr_4, ptr);
    ASSERT_NOT_EQ(ptr_4, ptr_2);
    ASSERT_NOT_EQ(ptr_4, ptr_3);
}

void test_gc_collect_collect(){
    slab_reset();
    slab_init();
    root_list = NULL;
    allocation_list = NULL;
    void* ptr = gc_alloc(8); 
    void* ptr_2 = gc_alloc(8);
    void* ptr_3 = gc_alloc(8); 
    void* ptr_4 = gc_alloc(16);
    int err_2 = gc_register_root(ptr);
    int err_3 = gc_register_root(ptr_2);
    int err_4 = gc_register_root(ptr_3);
    ASSERT_EQ(err_2, 0);
    ASSERT_EQ(err_3, 0);
    ASSERT_EQ(err_4, 0);
    //verify mark state 
    allocation_t* scan = allocation_list;
    int rooted_before = 0;
    int unrooted_before = 0;
    while (scan != NULL) {
        ASSERT_EQ(scan->marked, 0);
        if (scan->block == ptr || scan->block == ptr_2 || scan->block == ptr_3) {
            rooted_before++;
        } else if (scan->block == ptr_4) {
            unrooted_before++;
        }
        scan = scan->next;
    }
    ASSERT_EQ(rooted_before, 3);
    ASSERT_EQ(unrooted_before, 1);
    int err = gc_collect();
    ASSERT_EQ(err, 0);
    // verify sweep result 
    allocation_t* curr = allocation_list;
    int survivor_count = 0;
    int found_ptr = 0;
    int found_ptr_2 = 0;
    int found_ptr_3 = 0;
    int found_ptr_4 = 0;
    while (curr != NULL) {
        survivor_count++;
        ASSERT_EQ(curr->marked, 0); // gc_collect resets marks on survivors
        if (curr->block == ptr) found_ptr = 1;
        else if (curr->block == ptr_2) found_ptr_2 = 1;
        else if (curr->block == ptr_3) found_ptr_3 = 1;
        else if (curr->block == ptr_4) found_ptr_4 = 1;
        curr = curr->next;
    }
    ASSERT_EQ(survivor_count, 3);
    ASSERT_EQ(found_ptr, 1);
    ASSERT_EQ(found_ptr_2, 1);
    ASSERT_EQ(found_ptr_3, 1);
    ASSERT_EQ(found_ptr_4, 0);
}

void test_gc_collect_verify_blocks_reused() {
    slab_reset();
    slab_init();
    root_list = NULL;
    allocation_list = NULL;
    void* keep = gc_alloc(8);
    void* to_collect = gc_alloc(8);
    ASSERT_PTR_NOT_NULL(keep);
    ASSERT_PTR_NOT_NULL(to_collect);
    int err = gc_register_root(keep);
    ASSERT_EQ(err, 0);
    err = gc_collect();
    ASSERT_EQ(err, 0);
    void* reused = gc_alloc(8);
    ASSERT_PTR_NOT_NULL(reused);
    ASSERT_EQ(reused, to_collect);
}
int main(void) {
    
    RUN_TEST(test_slab_init);
    RUN_TEST(test_slab_alloc_no_nodes_on_slab);
    RUN_TEST(test_slab_free_invalid_inputs);
    RUN_TEST(test_slab_free_slabs_not_inited);
    RUN_TEST(test_slab_free_no_existing_free_list);
    RUN_TEST(test_slab_free_existing_free_list);
    RUN_TEST(test_slab_alloc_nodes_exist_on_slab);
    RUN_TEST(test_slab_stress);
    RUN_TEST(test_gc_collect_register_root);
    RUN_TEST(test_gc_collect_unregistered_root);
    RUN_TEST(test_gc_collect_collect);
    RUN_TEST(test_gc_collect_verify_blocks_reused);
    //slab_stats();
    //gc_stats();
    return SU_report_and_return();
    
    
}
