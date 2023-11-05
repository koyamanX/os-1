#include <list.h>

void list_init(list_t *list) {
    list->prev = list;
    list->next = list;
}

void list_push_back(list_t *list, list_elem_t *elem) {
    elem->prev = list->prev;
    elem->next = list;
    list->prev->next = elem;
    list->prev = elem;
}

list_elem_t *list_pop_front(list_t *list) {
    list_elem_t *elem = list->next;
    list->next = elem->next;
    elem->next->prev = list;
    return elem;
}
