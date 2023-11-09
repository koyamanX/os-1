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
    if(LIST_EMPTY(list)) {
        return NULL;
    }
    list_elem_t *elem = list->next;
    if(list_remove(elem) == 0) {
        return NULL;
    }

    return elem;
}

int list_remove(list_elem_t *elem) {
    if(elem->prev == NULL || elem->next == NULL) {
        return 0;
    }
    elem->prev->next = elem->next;
    elem->next->prev = elem->prev;

    elem->prev = NULL;
    elem->next = NULL;

    return 1;
}
