#include <list.h>

void list_init(list_t *list) {
    list->prev = list;
    list->next = list;
}

void list_push_back(list_t *list, list_elem_t *elem) {
    elem->prev = list->prev;
    elem->next = list->next;
    list->prev = elem;
    list->next = elem;
}

list_elem_t *list_pop_front(list_t *list) {
    list_t *head = list->next;
    
    if(head == list) {
        return NULL;
    }

    list_t *next = head->next;
    list->next = next;
    next->prev = list;

    head->prev = NULL;
    head->next = NULL;

    return head;
}
