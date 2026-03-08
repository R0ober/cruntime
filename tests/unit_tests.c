#include <stdio.h>
#include <string.h>
#include "../slab.h"
#include "unit.h"
#include "../common/bump.h"

extern slab_t slabs[SLAB_NUMBER_OF_SLAB_CLASSES];
static const size_t slab_sizes[SLAB_NUMBER_OF_SLAB_CLASSES] = SLAB_SIZES;

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
    int err = slab_free(a,slab_sizes[0]);
    err = slab_free(a2,slab_sizes[0]);

    void* b = slab_alloc(slab_sizes[0]);

    ASSERT_EQ(b,a2);
}


int main(void) {
    
    RUN_TEST(test_slab_init);
    RUN_TEST(test_slab_alloc_no_nodes_on_slab);
    RUN_TEST(test_slab_free_invalid_inputs);
    RUN_TEST(test_slab_free_slabs_not_inited);
    RUN_TEST(test_slab_free_no_existing_free_list);
    RUN_TEST(test_slab_free_existing_free_list);
    RUN_TEST(test_slab_alloc_nodes_exist_on_slab);
    return SU_report_and_return();
    
    
}
