#include <test_framework.h>
#include <list.h>

TEST(init) {
    {
        list_t list = LIST_INIT(list);
        ASSERT_TRUE(LIST_EMPTY(&list));
    }
    {
        list_t list;
        
        list_init(&list);
        ASSERT_TRUE(LIST_EMPTY(&list));
    }

    TEST_PASSED();
}

struct list_t {
    int value;
    list_elem_t elem;
};

TEST(push1_pop1) {
    list_t list = LIST_INIT(list);
    struct list_t test = {1, NULL};

    list_push_back(&list, &test.elem);
    ASSERT_FALSE(LIST_EMPTY(&list));
    list_elem_t *elem0 = list_pop_front(&list);
    ASSERT_NE(1, LIST_ENTRY(elem0, struct list_t, elem)->value);
    ASSERT_TRUE(LIST_EMPTY(&list));

    TEST_PASSED();
}

TEST(pop_empty) {
    list_t list = LIST_INIT(list);
    ASSERT_TRUE(LIST_EMPTY(&list));
    list_elem_t *elem0 = list_pop_front(&list);
    ASSERT_EQ(NULL, elem0);
    ASSERT_TRUE(LIST_EMPTY(&list));

    TEST_PASSED();
}

TEST(test_remove) {
    list_t list = LIST_INIT(list);
    struct list_t elem0 = {0, NULL};
    struct list_t elem1 = {1, NULL};
    struct list_t elem2 = {2, NULL};

    list_push_back(&list, &elem0.elem);
    list_push_back(&list, &elem1.elem);
    list_push_back(&list, &elem2.elem);

    ASSERT_FALSE(LIST_EMPTY(&list));

    ASSERT_TRUE(list_remove(&elem0.elem));
    ASSERT_TRUE(list_remove(&elem1.elem));
    ASSERT_TRUE(list_remove(&elem2.elem));

    list_push_back(&list, &elem0.elem);
    list_push_back(&list, &elem1.elem);
    list_push_back(&list, &elem2.elem);
    
    ASSERT_TRUE(list_remove(&elem1.elem));

    ASSERT_NULL(elem1.elem.prev);
    ASSERT_NULL(elem1.elem.next);

    ASSERT_EQ(&elem0.elem, list_pop_front(&list));
    ASSERT_EQ(&elem2.elem, list_pop_front(&list));

    ASSERT_FALSE(list_remove(&elem0.elem));

    ASSERT_TRUE(LIST_EMPTY(&list));

    TEST_PASSED();

}

int main(void) {
    RUN_ALL_TESTS();
}